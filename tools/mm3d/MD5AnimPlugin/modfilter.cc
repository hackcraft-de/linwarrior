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
#include <cassert>


using std::list;
using std::string;
using std::vector;
using std::map;
using std::set;



Filtername::Filtername()
{
   //m_read.push_back( "md5anim" );
   m_write.push_back( "md5anim" );
   //m_export.push_back( "md5anim" );
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
   //vector< int > jmap;
   jmap.reserve( joints.size() );
   
   // Inverse of jmap.
   //vector< int > jmap_inv;
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
   stack.push_back( -1 );
   // Now run through the hierarchy and map:
   unsigned jointcnt = 0;
   while (!stack.empty())
   {
      int jointidx = stack.back();
      stack.pop_back();
      // Map that pre-order idx to it's joint;
      if (jointidx != -1) {
         jmap[jointcnt] = jointidx;
         jmap_inv[jointidx] = jointcnt;
         jointcnt++;
      }
      // Find the children for that joint idx:
      for (unsigned i = 0; i < joints.size(); i++)
      {
         if (joints[i]->m_parent == jointidx)
            stack.push_back( i );
      }
   }
#endif   
}


void Filtername::printHierarchy(DataDest* dst, Model* model, vector< int >& jmap, vector< int >& jmap_inv, vector< unsigned >& components)
{
   dst->writePrintf( "hierarchy {\n" );

   vector< Model::Joint* >& joints = getJointList( model );

   // BEGIN OF OUTPUT
   unsigned compcnt = 0;
   for (unsigned i = 0; i < joints.size(); i++)
   {
      unsigned jointidx = jmap[i];
      int parentidx = joints[jointidx]->m_parent;      
      
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

      unsigned startindex = compcnt;

      // Animated components flags for (Tx Ty Tz Rx Ry Rz).
      int flags = components[jointidx];
      
      for (unsigned b = 0; b < 6; b++) if (flags & (1<<b)) compcnt++;
      
      // Not part of the format but for human deciphering.
      string comment = "";
      if (flags) {
         comment += "( ";
         if (flags &  1) comment += "Tx ";
         if (flags &  2) comment += "Ty ";
         if (flags &  4) comment += "Tz ";
         if (flags &  8) comment += "Qx ";
         if (flags & 16) comment += "Qy ";
         if (flags & 32) comment += "Qz ";
         comment += ")";
      }
      
      int parentidx_ = (parentidx < 0) ? -1 : jmap_inv[parentidx];
      dst->writePrintf( "\t\"%s\" %i %i %i \t\t// %s %s\n", cname.c_str(), parentidx_, flags, startindex, cparent.c_str(), comment.c_str() );
   }

   dst->writePrintf( "}\n" );
   dst->writePrintf( "\n" );
}


void Filtername::printBaseframe(DataDest* dst, Model* model, Matrix& axis, vector< int >& jmap, vector< unsigned >& components)
{
   dst->writePrintf( "baseframe {\n" );
   
   vector< Model::Joint* >& joints = getJointList( model );
   
   for (unsigned i = 0; i < joints.size(); i++)
   {
      unsigned jointidx = jmap[i];
      int parentidx = joints[jointidx]->m_parent;      
      
      // First transform Joint to our export axis system.            
      Matrix m;
      if (parentidx < 0) m = axis;
      m.postMultiply( joints[jointidx]->m_relative );
      
      // Now extract position and quaternion.
      double t[] = { 0,0,0,0 };
      Quaternion q;
      m.getTranslation( t );
      m.getRotationQuaternion( q );
      q = q.swapHandedness();

      // Make sure the bone's name is a valid identifier.
      string cname = joints[jointidx]->m_name;
      for (unsigned p = 0; p < cname.length(); p++)
      {
            if (!isalpha( cname[p] ) && !isdigit( cname[p]) ) cname[p] = '_';
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
      
      dst->writePrintf( "\t( %.4lf %.4lf %.4lf ) ( %.4lf %.4lf %.4lf )\n", t[0], t[1], t[2], q[0],q[1],q[2] );
   }

   dst->writePrintf( "}\n" );
   dst->writePrintf( "\n" );
}


void Filtername::printBounds(DataDest* dst, Model* model, Matrix& axis, int anim, vector< int >& jmap, vector< int >& jmap_inv)
{
   bool loop = model->isAnimationLooping();

   vector< Model::Joint* >& joints = getJointList( model );
   unsigned numjoints = joints.size();
   Model::SkelAnim* skelanim = getSkelList( model )[anim];
   
   vector< Matrix > absolute;
   absolute.reserve(numjoints);
   absolute.resize(numjoints);
   
   dst->writePrintf( "bounds {\n" );
   
   // Output data for the animated components for every frame.
   for (unsigned i = 0; i < getSkelList( model )[anim]->m_frameCount; i++)
   {
      unsigned frame = i;
      double frametime = frame * skelanim->m_spf;
      
      //float max = std::numeric_limits<float>::max();
      //float min = -std::numeric_limits<float>::max();
      float max = +1e+10;
      float min = -1e+10;
      float minmax[] = { max,max,max, min,min,min };
      float* e = minmax;
      //dst->writePrintf( "\t//( %.4f %.4f %.4f ) ( %.4f %.4f %.4f )\n", e[0],e[1],e[2], e[3],e[4],e[5] );

      // For every joint that contains animated components.
      for (unsigned j = 0; j < numjoints; j++)
      {
         unsigned jointidx = jmap[j];
         int parentidx = joints[jointidx]->m_parent;
         
         Matrix relani;
         model->interpSkelAnimKeyframeTime( anim, frametime, loop, jointidx, relani );
         
         Matrix relative;
         relative = relani * joints[jointidx]->m_relative;

         // transform Joint matrix to our export axis system.
         Matrix m;
         if (parentidx < 0) m = axis;
         m.postMultiply( relative );
         relative = m;
         
         if (parentidx >= 0) absolute[j] = relative * absolute[jmap_inv[parentidx]];
         else absolute[j] = relative;
         
         double v[] = { 0,0,0,0 };
         absolute[j].getTranslation(v);
         //dst->writePrintf( "\t// ( %.4f %.4f %.4f )\n", v[0],v[1],v[2] );
         minmax[0] = (v[0] < minmax[0]) ? v[0] : minmax[0];
         minmax[1] = (v[1] < minmax[1]) ? v[1] : minmax[1];
         minmax[2] = (v[2] < minmax[2]) ? v[2] : minmax[2];
         minmax[3] = (v[0] > minmax[3]) ? v[0] : minmax[3];
         minmax[4] = (v[1] > minmax[4]) ? v[1] : minmax[4];
         minmax[5] = (v[2] > minmax[5]) ? v[2] : minmax[5];
      }
      dst->writePrintf( "\t( %.4f %.4f %.4f ) ( %.4f %.4f %.4f )\n", e[0],e[1],e[2], e[3],e[4],e[5] );
   }
   dst->writePrintf( "}\n" );
   dst->writePrintf( "\n" );
}


void Filtername::printFrames(DataDest* dst, Model* model, Matrix& axis, int anim, vector< int >& jmap, vector< int >& jmap_inv, vector< unsigned >& components)
{
   double p[6];
   bool loop = model->isAnimationLooping();

   vector< Model::Joint* >& joints = getJointList( model );
   unsigned numjoints = joints.size();
   Model::SkelAnim* skelanim = getSkelList( model )[anim];
   
   // Output data for the animated components for every frame.
   for (unsigned i = 0; i < getSkelList( model )[anim]->m_frameCount; i++)
   {
      unsigned frame = i;
      double frametime = frame * skelanim->m_spf;
      dst->writePrintf( "frame %i {\n", frame );

      // For every joint that contains animated components.
      for (unsigned j = 0; j < numjoints; j++)
      {
         unsigned jointidx = jmap[j];
         int parentidx = joints[jointidx]->m_parent;
         
         Matrix relani;
         model->interpSkelAnimKeyframeTime( anim, frametime, loop, jointidx, relani );
         
         Matrix relative;
         relative = relani * joints[jointidx]->m_relative;

         // transform Joint matrix to our export axis system.
         Matrix m;
         if (parentidx < 0) m = axis;
         m.postMultiply( relative );
         
         // Now extract position and quaternion.
         double u[] = { 0,0,0,0 };
         Quaternion q;
         m.getTranslation( u );
         m.getRotationQuaternion( q );
         q = q.swapHandedness();

         p[0] = u[0];
         p[1] = u[1];
         p[2] = u[2];
         p[3] = q[0];
         p[4] = q[1];
         p[5] = q[2];
         
         // Has this joint animated components to print?
         if (components[jointidx] != 0) {
            dst->writePrintf( "\t" );
            for (unsigned b = 0; b < 6; b++)
            {
               if (components[jointidx] & (1<<b))
               {
                  dst->writePrintf( " %.4f", p[b] );
               }
            }
            dst->writePrintf( "\n" );
         }
         
      }
      
      dst->writePrintf( "}\n" );
      dst->writePrintf( "\n" );
   }
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

   // Get sure there is an animation to export.
   unsigned anim = 0;
   anim = model->getCurrentAnimation();
   if (getSkelList( model ).size() <= anim) return err;
   
   Model::SkelAnim* skelanim = getSkelList( model )[anim];

   // Flag for each joint each animated component and count them.
   // For each joint there are (Tx Ty Tz Qx Qy Qz) = 6 components possible.
   // mm3d currently doesn't support componentwise keyframes but
   // that may change - anyhow better: be prepared be prepared...
   std::vector< unsigned > components;
   components.reserve( getJointList( model ).size() );
   components.resize( getJointList( model ).size(), 0 );

   unsigned numcomp = 0;
   std::set< unsigned > compset;
   for (unsigned i = 0; i < skelanim->m_jointKeyframes.size(); i++)
   {
      for (unsigned j = 0; j < skelanim->m_jointKeyframes[i].size(); j++)
      {
         Model::Keyframe *keyframe = skelanim->m_jointKeyframes[i][j];
         unsigned joint = keyframe->m_jointIndex;
         if (!keyframe->m_isRotation) {
            components[joint] = 1 + 2 + 4; // Tx Ty Tz
            compset.insert( joint*6 +0 );
            compset.insert( joint*6 +1 );
            compset.insert( joint*6 +2 );
         } else {
            components[joint] = 8 + 16 + 32; // Qx Qy Qz
            compset.insert( joint*6 +3 );
            compset.insert( joint*6 +4 );
            compset.insert( joint*6 +5 );
         }
      }
   }
   numcomp = compset.size();

   // MD5ANIM Header
   dst->writePrintf( "MD5Version 10\n" );
   dst->writePrintf( "commandline \"\"\n" );
   dst->writePrintf( "\n" );
   dst->writePrintf( "numFrames %i\n", skelanim->m_frameCount );
   dst->writePrintf( "numJoints %i\n", getJointList( model ).size() );
   dst->writePrintf( "frameRate %i\n", (int)skelanim->m_fps );
   dst->writePrintf( "numAnimatedComponents %i\n", numcomp );
   dst->writePrintf( "\n" );
   dst->writePrintf( infoString );
   dst->writePrintf( "\n" );

   vector< int > jmap;
   vector< int > jmap_inv;
   orderJoints( model, jmap, jmap_inv );
   
   printHierarchy( dst, model, jmap, jmap_inv, components );
   
   printBounds( dst, model, axis, anim, jmap, jmap_inv );
   
   printBaseframe( dst, model, axis, jmap, components );
   
   printFrames( dst, model, axis, anim, jmap, jmap_inv, components );

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
         if ( strcasecmp( &filename[len-cmpstr.length()], cmpstr.c_str() ) == 0 )
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
   return "MD5Anim export by hackcraft.de";
}

#endif // PLUGIN

