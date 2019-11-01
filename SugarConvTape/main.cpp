//////////////////////////////////////////////////////////
// SugarConvTape
//
// Convert tape format supported by Sugarbox
//
// (c) T.Guillemin 2016 -
//
//
//////////////////////////////////////////////////////////


#include "../CPCCoreEmu/stdafx.h"
#include "Tape.h"

#ifdef __unix
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif // __unix

#include <string.h>
#include <regex>
#include <algorithm>

bool b_su_dir = false;
int type = 0;
char filter[MAX_PATH] = { 0 };


size_t ReplaceAll(std::string &str, const std::string &from, const std::string &to)
{
   size_t count = 0;

   size_t pos = 0;
   while ((pos = str.find(from, pos)) != std::string::npos)
   {
      str.replace(pos, from.length(), to);
      pos += to.length();
      ++count;
   }

   return count;
}

void EscapeRegex(std::string &regex)
{
   ReplaceAll(regex, "\\", "\\\\");
   ReplaceAll(regex, "^", "\\^");
   ReplaceAll(regex, ".", "\\.");
   ReplaceAll(regex, "$", "\\$");
   ReplaceAll(regex, "|", "\\|");
   ReplaceAll(regex, "(", "\\(");
   ReplaceAll(regex, ")", "\\)");
   ReplaceAll(regex, "[", "\\[");
   ReplaceAll(regex, "]", "\\]");
   ReplaceAll(regex, "*", "\\*");
   ReplaceAll(regex, "+", "\\+");
   ReplaceAll(regex, "?", "\\?");
   ReplaceAll(regex, "/", "\\/");
}

bool MatchTextWithWildcards(const std::string &text, std::string wildcardPattern /*, bool caseSensitive = true*/)
{
   // Escape all regex special chars
   EscapeRegex(wildcardPattern);

   // Convert chars '*?' back to their regex equivalents
   ReplaceAll(wildcardPattern, "\\?", ".");
   ReplaceAll(wildcardPattern, "\\*", ".*");

   std::regex pattern(wildcardPattern /*, caseSensitive ? std::regex::normal : std::regex::icase*/);

   return std::regex_match(text, pattern);
}

int ConversionFile(fs::path& pSource, fs::path& pDestination)
{
   int retValue = 0;
   char destination[MAX_PATH] = { 0 };

   ////////////////////////////////////
   // Destination
   if (pDestination.empty() )
   {
      // take source, remove file spec
      pDestination = pSource;
      pDestination.replace_extension("");
   }

   if (pDestination.has_extension() == false)
   {
      // Add proper extension
      switch (type)
      {
      case 0:
         pDestination.replace_extension(".wav");
         break;
      case 1:
      case 4:
         pDestination.replace_extension(".cdt");
         break;
      case 2:
      case 3:
         pDestination.replace_extension(".csw");
         break;
      }

   }

   printf("Loading %s", pSource.generic_string().c_str());

   CTape tape;
   // Load the tape
   if (tape.InsertTape(pSource.generic_string().c_str()) != 0)
   {
      printf("!!! Error loading tape\n");
      return -1;
   }
   else
   {
      printf(" ok - ");
   }
   // Add extension

   // Save with the correct format
   switch (type)
   {
   case 0:
      tape.SaveAsWav(pDestination.generic_string().c_str());
      break;
   case 1:
      tape.SaveAsCdtDrb(pDestination.generic_string().c_str());
      break;
   case 4:
      tape.SaveAsCdtCSW(pDestination.generic_string().c_str());
      break;
   case 2:
      tape.SaveAsCSW(pDestination.generic_string().c_str(), 1, 1);
      break;
   case 3:
      tape.SaveAsCSW(pDestination.generic_string().c_str(), 2, 2);
      break;
   }

   printf("Saved as %s\n", pDestination.generic_string().c_str());

   return retValue;
}

int ConversionDirectory(fs::path& source, fs::path& destination)
{

   printf("DIRECTORY => %s\n", source.generic_string().c_str());

   // -r and subdirs ?
   for (auto& p : fs::directory_iterator(source))
   {
      if (fs::is_regular_file(p.status()))
      {
         // Filter ? - TODO
         if (filter == nullptr || MatchTextWithWildcards(p.path().filename().generic_string(), filter))
         {
            fs::path input_file = p.path();
            fs::path output_file = destination;
            output_file.replace_filename(p.path().filename());
            output_file.replace_extension("");

            if (ConversionFile(input_file, output_file) == 1)
            {
               //End of this directory
               return 0;
            }
         }
      }
   }

   // Now, works on subdirs
   if (b_su_dir)
   {

      for (auto& p : fs::directory_iterator(source))
      {
         if (fs::is_directory(p.status()))
         {
            fs::path input_file = p.path();

            ConversionDirectory(input_file, destination);

            if (ConversionFile(input_file, destination) == 1)
            {
               //End of this directory
               return 0;
            }
         }

      }
   }

   return 0;
}


void PrintUsage()
{
   printf("Usage : SugarConvTape source [destination] [-o=outputformat] [-r] [-f=filter]\n");
   printf("\n");
   printf("    -o=outputformat : Select output format. Can take the following values:\n");
   printf("        wav : WAV format\n");
   printf("        cdt_drb : CDT with DRB\n");
   printf("        cdt_csw : CDT with CSW inside\n");
   printf("        csw11 : CSW 1.1\n");
   printf("        csw20 : CSW 2.0\n");
   printf("    If this parameter is not used, default output format is WAV\n");
   printf("\n");
   printf("    destination : output file. If source file is a directory, destination is used as an output directory\n");
   printf("\n");
   printf("    -r : If the source file is a directory, convert recursively the given directory.\n");
   printf("    -f=filter : If the source file is a directory, set a filter for the files to convert.\n");

}


int main(int argc, char** argv)
{
   int retValue = 0;
   ////////////////////////////////////
   // Sanity check :
   bool bCorrect = (argc > 1);

   char* pSource = nullptr;
   char* pDestination = nullptr;

   for (int i = 1; i < argc && bCorrect ; i++)
   {
      if (argv[i][0] == '-')
      {
         // Argument
         if (stricmp(argv[i], "-o=wav") == 0) type = 0;
         else if (stricmp(argv[i], "-o=cdt_drb") == 0) type = 1;
         else if (stricmp(argv[i], "-o=cdt_csw") == 0) type = 4;
         else if (stricmp(argv[i], "-o=csw11") == 0) type = 2;
         else if (stricmp(argv[i], "-o=csw20") == 0) type = 3;
         else if (stricmp(argv[i], "-r") == 0)
         {
            b_su_dir = true;
         }
         else if (strnicmp(argv[i], "-f=", 3) == 0)
         {
            sscanf(argv[i], "-f=%s", filter);

         }

         else
         {
            // Unknown option
            bCorrect = false;
         }
      }
      else
      {
         // Source / destination
         if (pSource == NULL)
         {
            pSource = argv[i];
         }
         else if (pDestination == NULL)
         {
            pDestination = argv[i];
         }
         else
         {
            // Too much parameter !
            bCorrect = false;
         }
      }
   }

   if (!bCorrect)
   {
      PrintUsage();
      return -1;
   }
   if (pDestination == nullptr)
   {
      pDestination = "";
   }

   ////////////////////////////////////
   // Conversion
   fs::path source(pSource);
   fs::path destination(pDestination);

   if (fs::is_regular_file(fs::status(source)))
   {
      return ConversionFile(source, destination);
   }
   else if (fs::is_directory(fs::status(source)))
   {
      return ConversionDirectory(source, destination);
   }
}
