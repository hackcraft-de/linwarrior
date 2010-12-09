#include "modfilter.h"

//#include "model.h"
//#include "texture.h"
//#include "mm3dconfig.h"
#include "log.h"
//#include "binutil.h"
//#include "misc.h"
#include "version.h"
#include "datadest.h"
#include "datasource.h"
//#include "release_ptr.h"
#include "glmath.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <map>


using std::list;
using std::string;
using std::vector;
using std::map;



Filtername::Filtername()
{
   //m_read.push_back( "md5mesh" );
   m_write.push_back( "md5mesh" );
   //m_export.push_back( "md5mesh" );
}


Filtername::~Filtername()
{
}


void Filtername::release()
{
   delete this;
}


Model::ModelErrorE Filtername::readFile( Model * model, const char * const filename )
{
   // Open file for input:
   Model::ModelErrorE err = Model::ERROR_NONE;
   DataSource* src = openInput( filename, err );
   SourceCloser fc( src );
   // Return on error:
   if (err != Model::ERROR_NONE) return err;

   // TODO: Write your reader:

   return err;
}


void Filtername::orderJoints(Model* model, vector< int >& jmap, vector< int >& jmap_inv)
{
   vector< Model::Joint* >& joints = getJointList( model );

   // MAPPING BONE INDICES
   // It may not be sure that the joints are ordered in
   // parent first manner - ie. pre-order or level-order
   // of the bone-tree would be fine. Therefore map
   // bone idx to a pre-order idx. When animating the
   // model this makes sure that the parent gets transformed first.
   // The result is that walking through the final joint-list is
   // enough to get pre-order operations.

   // This will store the mapping from pre-ordered idx to un-ordered idx.
   // Defined as vector< int > jmap;
   jmap.reserve( joints.size() );
   
   // Inverse of jmap.
   // Defined as vector< int > jmap_inv;
   jmap_inv.reserve( joints.size() );

#if 1
   // Assume identity map
   for (unsigned i = 0; i < joints.size(); i++)
   {
      jmap[i] = i;
      jmap_inv[i] = i;
   }
#else
   // Use a stack to loop'ise the recursion.
   vector< unsigned > stack;
   stack.reserve(joints.size());
   stack.push_back(-1);
   // Now run through the hierarchy and map:
   unsigned jointcnt = 0;
   while (!stack.empty())
   {
      int jointidx = stack.back();
      stack.pop_back();
      // Map that pre-order idx to it's joint;
      if (jointidx != -1)
      {
         jmap[jointcnt] = jointidx;
         jmap_inv[jointidx] = jointcnt;
         jointcnt++;
      }
      // Find the children for that joint idx:
      for (unsigned i = 0; i < joints.size(); i++)
      {
         if (joints[i]->m_parent == jointidx)
            stack.push_back(i);
      }
   }
#endif   
}


void Filtername::printJoints(DataDest* dst, Model* model, Matrix& axis, vector< int >& jmap, vector< int >& jmap_inv)
{
   dst->writePrintf( "joints {\n" );

   vector< Model::Joint* >& joints = getJointList( model );

   // BEGIN OF OUTPUT
   
   unsigned numjoints = joints.size();

   for (unsigned i = 0; i < numjoints; i++)
   {
      unsigned jointidx = jmap[i];
      int parentidx = joints[jointidx]->m_parent;      
      
      // First transform Joint to our export axis system.            
      Matrix m = axis;
      m.postMultiply( joints[jointidx]->m_absolute );
      
      // Now extract position and quaternion.
      double t[] = { 0,0,0 };
      Quaternion q;
      m.getTranslation( t );
      m.getRotationQuaternion( q );
      q = q.swapHandedness();

      // Make sure the bone's name is a valid identifier.
      string cname = joints[jointidx]->m_name;
      for (unsigned p = 0; p < cname.length(); p++)
      {
            if (!isalpha( cname[p] ) && !isdigit( cname[p] )) cname[p] = '_';
      }
      
      // Make sure the parent's name is a valid identifier.
      // Note this is just for commenting the parent -
      // not part of the format but may be used to decifer it.
      string cparent;
      if (parentidx >= 0)  cparent = joints[parentidx]->m_name;
      for (unsigned p = 0; p < cparent.length(); p++)
      {
            if (!isalpha( cparent[p] ) && !isdigit( cparent[p] )) cparent[p] = '_';
      }
      
      int parentidx_ = (parentidx < 0) ? -1 : jmap_inv[parentidx];
      dst->writePrintf( "\t\"%s\" %i ( %.4lf %.4lf %.4lf ) ( %.4lf %.4lf %.4lf )\t\t// %s\n", cname.c_str(), parentidx_, t[0], t[1], t[2], q[0],q[1],q[2], cparent.c_str() );
   }

   dst->writePrintf( "}\n" );
   dst->writePrintf( "\n" );
}

void Filtername::printMesh(DataDest* dst, Model* model, Model::Group* group, Matrix& axis, vector< int >& jmap_inv)
{
   dst->writePrintf( "mesh {\n" );
   dst->writePrintf( "\t// meshes: %s\n", group->m_name.c_str() );
   
   int matidx = group->m_materialIndex;
   const char* materialname = (matidx == -1) ? "none" : getMaterialList(model)[matidx]->m_name.c_str();
   dst->writePrintf( "\tshader \"%s\"\n", materialname );
   dst->writePrintf( "\n" );

   // BEGIN OF PREPROCESSING: MAPPING VERTEX INDICES
   // Search through all triangles in group (mesh) for all global vertex
   // indices and map each unique to a new local mesh vertex index++.
   // [bijective mapping between global vertex idx and mesh vertex idx]
   //

   // vertmap maps global model vtx idx to local mesh vtx idx.
   std::map< unsigned, unsigned > vertmap;
   
   // inverse of vertmap: local to global.
   std::map< unsigned, unsigned > vertmap_inv;

   // Mapping of local vertex idx to texture coords.
   // Note that we reserve a worst case count:
   // A model with several meshes may use less vertices.
   std::vector< float > umap;
   std::vector< float > vmap;
   unsigned maxworstvtx = getVertexList( model ).size();
   vmap.reserve( maxworstvtx );
   umap.reserve( maxworstvtx );
   
   unsigned vertcnt = 0;
   unsigned numtris = group->m_triangleIndices.size();
   for (std::set<int>::iterator it = group->m_triangleIndices.begin(); it != group->m_triangleIndices.end(); it++)
   {
      unsigned triidx = *it;
      Model::Triangle* tri = getTriangleList( model )[triidx];
      unsigned* vertidx = tri->m_vertexIndices;
      // For all 3 vertices of the triangle.
      for (unsigned i = 0; i < 3; i++)
      {
         // If this vertex is not yet mapped then map to next free index.
         if (vertmap.find( vertidx[i] ) == vertmap.end())
         {
            vertmap[vertidx[i]] = vertcnt;
            vertmap_inv[vertcnt] = vertidx[i];
            umap[vertcnt] = tri->m_s[i];
            vmap[vertcnt] = 1.0f - tri->m_t[i]; // Texture flipped vertical.
            vertcnt++;
         }
      }
   }
   
   //
   // END OF PREPROCESSING.

   // Output Mesh vertices
   //

   unsigned numverts = vertmap.size();
   dst->writePrintf( "\tnumverts %i\n", numverts );
   unsigned numweights = 0;
   for (unsigned i = 0; i < numverts; i++)
   {
      // vert vertex_index ( texture_u_coord texture_v_coord ) first_weight_index number_of_weights
      unsigned globalvert = vertmap_inv[i];
      Model::Vertex* vert = getVertexList( model )[globalvert];
      unsigned countweights = vert->m_influences.size();
      // even if no joint there must be a weight (joint idx -1 allowed anyway?)
      countweights = countweights == 0 ? 1 : countweights;
      dst->writePrintf( "\tvert %i ( %.3f %.3f ) %i %i\n", i, umap[i], vmap[i], numweights, countweights );
      numweights += countweights;
   }
   dst->writePrintf("\n");
   
   // Output Mesh triangles
   //

   dst->writePrintf( "\tnumtris %i\n", numtris );
   unsigned i = 0;
   for (std::set<int>::iterator it = group->m_triangleIndices.begin(); it != group->m_triangleIndices.end(); it++)
   {
      unsigned triidx = *it;
      Model::Triangle* tri = getTriangleList(model)[triidx];
      unsigned* vertidx = tri->m_vertexIndices;
      // tri triangle_index vertex_a_index vertex_b_index vertex_c_index
      dst->writePrintf( "\ttri %i %i %i %i\n", i, vertmap[vertidx[2]], vertmap[vertidx[1]], vertmap[vertidx[0]] );
      i++;
   }
   dst->writePrintf( "\n" );

   // Output Mesh weights
   //

   dst->writePrintf( "\tnumweights %i\n", numweights );
   numweights = 0;
   for (unsigned i = 0; i < numverts; i++)
   {
      // vert vertex_index ( texture_u_coord texture_v_coord ) first_weight_index number_of_weights
      unsigned globalvert = vertmap_inv[i];
      Model::Vertex* vert = getVertexList(model)[globalvert];
      // even if without joint there must be a weight (joint idx -1 allowed anyway?)
      if (vert->m_influences.size() == 0)
      {
         dst->writePrintf( "\tweight %i %i %.3f ( %.3f %.3f %.3f )\n", numweights, -1, 1.0f, vert->m_coord[0], vert->m_coord[1], vert->m_coord[2] );
         numweights++;
      }
      else
      {
         // Calculate total weight's magnitude.
         float weightsum = 0.0f;
         for (std::list<Model::InfluenceT>::iterator it = vert->m_influences.begin(); it != vert->m_influences.end(); it++) {
            Model::InfluenceT& in = *it;
            float bias = in.m_weight;
            weightsum += bias;
         }
         // Output weights with right proportion of weight.
         float remainder = 1.0f;
         for (std::list<Model::InfluenceT>::iterator it = vert->m_influences.begin(); it != vert->m_influences.end(); it++) {
            Model::InfluenceT& in = *it;
            int jointidx = in.m_boneId;
            Model::Joint* joint = getJointList( model )[jointidx];

            // Calculate right proportion so that the sum of all is 1.0f.
            float bias = in.m_weight;
            if (in.m_type == Model::IT_Remainder) bias = remainder;
            else if (bias == 0.0 && weightsum == 0.0) bias = 1.0f / (float) vert->m_influences.size();
            else bias /= weightsum;
            remainder -= bias;
            
            // Calculate Bone local vertex through bone absolute inverse:
            float v[] = { vert->m_coord[0], vert->m_coord[1], vert->m_coord[2] };
            // First transform vertex to our export axis system.
            axis.apply3( v );

            // Second transform Joint to our export axis system.            
            Matrix m = axis;
            m.postMultiply( joint->m_absolute );
            
            // Finally make the vertex local to it's joint.
            //
            
            // Apply inverse translation.
            double t[] = { 0,0,0,0 };
            m.getTranslation( t );
            v[0] += -t[0];
            v[1] += -t[1];
            v[2] += -t[2];
            
            // Apply inverse rotation.
            Quaternion q;
            m.getRotationQuaternion( q );
            q = q.swapHandedness();
            Matrix mr;
            mr.setRotationQuaternion( q );
            mr.apply3( v );
                        
            dst->writePrintf( "\tweight %i %i %.3f ( %.3f %.3f %.3f )\n", numweights, jmap_inv[jointidx], bias, v[0],v[1],v[2] );
            numweights++;
         }
      }
   }

   dst->writePrintf( "}\n" );
   dst->writePrintf( "\n" );
}


Model::ModelErrorE Filtername::writeFile( Model * model, const char * const filename, ModelFilter::Options * )
{
   const char* infoString =
      "// --------------------------------------- \n"
      "// exporter by B. Pickhardt (hackcraft.de) \n"
      "// --------------------------------------- \n";

   setlocale(LC_NUMERIC, "C");

   // Open file for output:
   Model::ModelErrorE err = Model::ERROR_NONE;
   DataDest* dst = openOutput( filename, err );
   DestCloser fc( dst );
   // Return on error:
   if (err != Model::ERROR_NONE) return err;
   
   // MD5Mesh and such are "Floor" oriented (CAD?)
   // therefore the floor is a XY-plane with Z for floors.
   Matrix axis;
   axis.loadIdentity();
   // Swap Y and Z axes (it's actually a rotation around x).
   if (1) {
      axis.set(1, 1, 0);
      axis.set(1, 2, 1);
      axis.set(2, 1, -1);
      axis.set(2, 2, 0);
   }

   // TODO: Write your writer:
   dst->writePrintf( "MD5Version 10\n" );
   dst->writePrintf( "commandline \"\"\n" );
   dst->writePrintf( "\n" );
   dst->writePrintf( "numJoints %i\n", getJointList(model).size() );
   dst->writePrintf( "numMeshes %i\n", getGroupList(model).size() );
   dst->writePrintf( "\n" );
   dst->writePrintf( infoString );
   dst->writePrintf( "\n" );
   
   vector< int > jmap;
   vector< int > jmap_inv;
   orderJoints(model, jmap, jmap_inv);
   
   printJoints(dst, model, axis, jmap, jmap_inv);
   
   for (unsigned i = 0; i < getGroupList(model).size(); i++)
   {
      printMesh(dst, model, getGroupList(model)[i], axis, jmap_inv);
   }

   return err;
}


bool Filtername::canRead( const char * filename )
{
   // If NULL then this is just to check general read capability.
   if (filename == NULL) return true;
   return knowsExtension( m_read, filename );
}


bool Filtername::canWrite( const char * filename )
{
   // If NULL then this is just to check general write capability.
   if (filename == NULL) return true;
   return knowsExtension( m_write, filename );
}


bool Filtername::canExport( const char * filename )
{
   // If NULL then this is just to check general export capability.
   if (filename == NULL) return true;
   return knowsExtension( m_export, filename );
}


bool Filtername::isSupported( const char * filename )
{
	return canRead( filename ) || canWrite( filename ) || canExport( filename );
}


list<string> Filtername::getReadTypes()
{
   return listExtensions( m_read );
}


list<string> Filtername::getWriteTypes()
{
   return listExtensions( m_write );
}


bool Filtername::knowsExtension( list< string >& extensions, const char * filename )
{
   if ( filename == NULL )
   {
      return false;
   }

   string cmpstr;
   unsigned len = strlen( filename );

   list<string>::iterator it;
   
   for ( it = extensions.begin(); it != extensions.end(); it++ )
   {
      cmpstr = string( "." ) + *it;

      if ( len >= cmpstr.length() )
      {
         if ( strcasecmp( &filename[len-cmpstr.length()], cmpstr.c_str()) == 0 )
         {
            return true;
         }
      }
   }

   return false;
}


list< string > Filtername::listExtensions( list< string >& extensions )
{
   list<string> rval;
   list<string>::iterator it;
   for ( it = extensions.begin(); it != extensions.end(); it++ )
   {
      rval.push_back( string( "*." ) + *it );
   }
   return rval;
}


// This ifdef is present in case someone wants to link the filter statically
// to Misfit Model 3D (or another program that uses the FilterManager)
//
// If your plugin will always behave as a plugin, you don't need to worry about
// this.
//
#ifdef PLUGIN

//------------------------------------------------------------------
// Plugin private data
//------------------------------------------------------------------

static Filtername * s_filter = NULL;

//------------------------------------------------------------------
// Plugin functions
//------------------------------------------------------------------

#include "filtermgr.h"
#include "pluginapi.h"

PLUGIN_API bool plugin_init()
{
   if ( s_filter == NULL )
   {
      s_filter = new Filtername();
      FilterManager * mgr = FilterManager::getInstance();
      mgr->registerFilter( s_filter );
   }
   log_debug( "MD5Mesh model filter plugin initialized\n" );
   return true;
}

// The filter manager will delete our registered filter.
// We have no other cleanup to do
PLUGIN_API bool plugin_uninit()
{
   s_filter = NULL; // FilterManager deletes filters
   log_debug( "MD5Mesh model filter plugin uninitialized\n" );
   return true;
}

PLUGIN_API const char * plugin_version()
{
   return "0.1.2 (2009_06_20)";
}

PLUGIN_API const char * plugin_mm3d_version()
{
   return VERSION_STRING;
}

PLUGIN_API const char * plugin_desc()
{
   return "MD5Mesh export by hackcraft.de";
}

#endif // PLUGIN

