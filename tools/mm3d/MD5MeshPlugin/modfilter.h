#ifndef __MODFILTER_H
#define __MODFILTER_H

// MD5Mesh Export Plugin for Misfit Model 3D

#include "modelfilter.h"
#include "datadest.h"
#include "datasource.h"
#include <string>
#include <vector>

#define Filtername MD5MESHPlugin


class Filtername : public ModelFilter
{
   public:

      /// TODO: See constructor for supported fileformats and add your code.
      Filtername();

      /// TODO: Clean-up after yourself as necessary.
      virtual ~Filtername();

   public: // Necessary PLUGIN methods (Abstract in ModelFilter):

      /// Invokes deletion of the plugin-instance (delete this).
      virtual void release();

      /// TODO: Implement this method if you want to read a fileformat.
      Model::ModelErrorE readFile( Model * model, const char * const filename );

      /// TODO: Implement this method if you want to write a fileformat.
      Model::ModelErrorE writeFile( Model * model, const char * const filename, ModelFilter::Options * );

      /// TODO: Returns true if this filter can read [the given file (-extension)].
      bool canRead( const char * filename = NULL);

      /// TODO: Returns true if this filter can write [the given file (-extension)].
      bool canWrite( const char * filename = NULL);

      /// TODO: Returns true if this filter can export [the given file (-extension)].
      bool canExport( const char * filename = NULL);

      /// Any support for fileformat? canRead or canWrite or canExport?
      bool isSupported( const char * filename );

      /// Returns list of "*.EXT" for all read-supported file-extensions.
      std::list< std::string > getReadTypes();

      /// Returns list of "*.EXT" for all write-supported file-extensions.
      std::list< std::string > getWriteTypes();

   protected: // Plugin utility methods:

      /// UTILITY: Returns true iff the filename matches any listed extension.
      bool knowsExtension( list< std::string >& extensions, const char * filename );

      /// UTILITY: Returns a list containing *.EXT for all extensions EXT listed.
      std::list< std::string > listExtensions( list< std::string >& extensions );

   protected: // Plugin utility members for utility methods:

      /// UTILITY: File format extensions supported for reading.
      std::list< std::string > m_read;

      /// UTILITY: File format extensions supported for writing.
      std::list< std::string > m_write;

      /// UTILITY: File format extensions supported for export.
      std::list< std::string > m_export;

   protected: // TODO: Your own methods and members:

      void orderJoints(Model* model, std::vector< int >& jmap, std::vector< int >& jmap_inv);
      void printJoints(DataDest* dst, Model* model, Matrix& axis, std::vector< int >& jmap, std::vector< int >& jmap_inv);
      void printMesh(DataDest* dst, Model* model, Model::Group* group, Matrix& axis, std::vector< int >& jmap_inv);

   // ...

};

#endif // MODFILTER_H

