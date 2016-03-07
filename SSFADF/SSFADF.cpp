// SSFADF.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tinyxml.h"
#define SCALE 100
#define CONSOLE_COLOR_NORMAL 7
#define CONSOLE_COLOR_RED 12

#include <direct.h>
#define GetCurrentDir _getcwd

#define DIR_TYPE 16384
#define FILE_TYPE 32768

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{

	HANDLE  hConsole;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  

	 char cCurrentPath[FILENAME_MAX];

	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
		{
		return errno;
		}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

	vector<stringPaths_t> all_paths = vector<stringPaths_t>(50);
	all_paths.clear();
	get_all_files_by_extension(&all_paths, cCurrentPath, ".hkx", true);

	printf("all_paths size: %d\n", all_paths.size());

	// Need to have memory allocated for the solver. Allocate 1mb for it.
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(1024 * 1024) );
	hkBaseSystem::init( memoryRouter, errorReport );
	//hkVersionPatchManager patchManager;
	//CustomRegisterPatches(patchManager);
	//hkDefaultClassNameRegistry &defaultRegistry = hkDefaultClassNameRegistry::getInstance();
	//CustomRegisterDefaultClasses();
	//ValidateClassSignatures();

	for (vector<stringPaths_t>::iterator it = all_paths.begin(); it != all_paths.end(); ++it)
	{
		printf("-----------------------------------------------\n");
		stringstream ss;

		TiXmlDocument doc("hkx");
		TiXmlElement * top_node = new TiXmlElement("anim");
		TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
		
		doc.LinkEndChild(decl);
		doc.LinkEndChild(top_node);

		hkIstream stream(it->original.c_str());
		hkStreamReader *reader = stream.getStreamReader();
		
		hkVariant root;
		hkResource *resource;

		hkResult res = hkSerializeLoad(reader, root, resource);

		if (!res.isSuccess())
		{
			cout << "bad file" << endl;
			return 1;
		}
		//cout << root.m_class->getName() << endl;

		hkRootLevelContainer * scene = resource->getContents<hkRootLevelContainer>();

		if (!scene)
		{
			SetConsoleTextAttribute(hConsole, CONSOLE_COLOR_RED);
			printf("SKIPPING NON_ANIMATION HKX FILE: \n");
			SetConsoleTextAttribute(hConsole, rand());
			printf("      %s\n", it->original);
			SetConsoleTextAttribute(hConsole, CONSOLE_COLOR_NORMAL);
			continue;
		}

		hkaAnimationContainer *ac = scene->findObject<hkaAnimationContainer>();

		hkaAnimation * m_animation = ac->m_animations[0];
		hkaAnimationBinding * m_binding = ac->m_bindings[0];

		TiXmlElement * data = new TiXmlElement("data");
		
		int FrameNumber = m_animation->getNumOriginalFrames();
		int TrackNumber = m_animation->m_numberOfTransformTracks;
		int FloatNumber = m_animation->m_numberOfFloatTracks;

		float AnimDuration = m_animation->m_duration;
		hkReal incrFrame = m_animation->m_duration / (hkReal)(FrameNumber-1);

		data->SetAttribute("FrameNumber", FrameNumber);
		data->SetAttribute("TrackNumber", TrackNumber);
		data->SetAttribute("FloatNumber", FloatNumber);
		
		ss << fixed << setprecision(8) <<  AnimDuration;
		data->SetAttribute("AnimDuration", ss.str().c_str());
		ss.str("");
		data->SetAttribute("numAnimKeys", FrameNumber * TrackNumber);
		ss << fixed << setprecision(8) <<  incrFrame;
		data->SetAttribute("incFrame", ss.str().c_str());
		ss.str("");
		top_node->LinkEndChild(data);

		cout << "no of frames: " << FrameNumber << endl;
		cout << "no of ttracks: " << TrackNumber << endl;
		cout << "no of ftracks: " << FloatNumber << endl;
		cout << "anim duration: " << AnimDuration << endl;
		cout << "incrFrame: " << incrFrame << endl;

		hkLocalArray<float> floatsOut(FloatNumber);
		hkLocalArray<hkQsTransform> transformOut(TrackNumber);
		hkQsTransform motionOut;
		floatsOut.setSize(FloatNumber);
		transformOut.setSize(TrackNumber);
		hkReal startTime = 0.0;

		hkArray<hkInt16> tracks;
		tracks.setSize(TrackNumber);
		for (int i=0; i<TrackNumber; ++i) tracks[i]=i;

		hkReal time = startTime;

		printf("refcount: %d\n", m_animation->getReferenceCount());

		for (int iFrame=0; iFrame<FrameNumber; ++iFrame, time += incrFrame)
		{
			
			m_animation->samplePartialTracks(time, TrackNumber, transformOut.begin(), FloatNumber, floatsOut.begin());
			hkaSkeletonUtils::normalizeRotations(transformOut.begin(), TrackNumber);
			
			m_animation->getExtractedMotionReferenceFrame(time, motionOut);
			printf("motion: (%f, %f, %f)\n", motionOut.getTranslation().getComponent(0), motionOut.getTranslation().getComponent(1), motionOut.getTranslation().getComponent(2));

			// assume 1-to-1 transforms
			// loop through animated bones
			for (int i=0; i<TrackNumber; ++i)
			{
				TiXmlElement * key = new TiXmlElement("key");
				hkQsTransform& transform = transformOut[i];		
				const hkVector4& anim_pos = transform.getTranslation();
				const hkQuaternion& anim_rot = transform.getRotation();

				// Translation 
				float px = anim_pos.getComponent(0);
				float py = anim_pos.getComponent(1);
				float pz = anim_pos.getComponent(2);

				px *= SCALE;
				py *= SCALE;
				pz *= SCALE;

				// Rotation
				float rx = anim_rot.m_vec.getComponent(0);
				float ry = anim_rot.m_vec.getComponent(1);
				float rz = anim_rot.m_vec.getComponent(2);
				float rw = anim_rot.m_vec.getComponent(3);

				ss.str("");

				ss << fixed << setprecision(8) <<  px;
				key->SetAttribute("px", ss.str().c_str()); ss.str("");
				ss << fixed << setprecision(8) <<  py;
				key->SetAttribute("py", ss.str().c_str()); ss.str("");
				ss << fixed << setprecision(8) <<  pz;
				key->SetAttribute("pz", ss.str().c_str()); ss.str("");
				ss << fixed << setprecision(8) <<  rx;
				key->SetAttribute("rx", ss.str().c_str()); ss.str("");
				ss << fixed << setprecision(8) <<  ry;
				key->SetAttribute("ry", ss.str().c_str()); ss.str("");
				ss << fixed << setprecision(8) <<  rz;
				key->SetAttribute("rz", ss.str().c_str()); ss.str("");
				ss << fixed << setprecision(8) <<  rw;
				key->SetAttribute("rw", ss.str().c_str()); ss.str("");
				data->LinkEndChild(key);
			}
		}

		doc.SaveFile(it->newer.c_str());
	}

	system("pause");

	return 0;
}

void HK_CALL errorReport(const char* msg, void* userContext)
{
   cout << msg << endl;
}


hkResult hkSerializeLoad(hkStreamReader *reader
                                , hkVariant &root
                                , hkResource *&resource)
{
   hkTypeInfoRegistry &defaultTypeRegistry = hkTypeInfoRegistry::getInstance();
   hkDefaultClassNameRegistry &defaultRegistry = hkDefaultClassNameRegistry::getInstance();

   hkBinaryPackfileReader bpkreader;
   hkXmlPackfileReader xpkreader;
   resource = NULL;
   hkSerializeUtil::FormatDetails formatDetails;
   hkSerializeUtil::detectFormat( reader, formatDetails );
   hkBool32 isLoadable = hkSerializeUtil::isLoadable( reader );
   if (!isLoadable && formatDetails.m_formatType != hkSerializeUtil::FORMAT_TAGFILE_XML)
   {
      return HK_FAILURE;
   }
   else
   {
      switch ( formatDetails.m_formatType )
      {
      case hkSerializeUtil::FORMAT_PACKFILE_BINARY:
         {
            bpkreader.loadEntireFile(reader);
            bpkreader.finishLoadedObjects(defaultTypeRegistry);
            if ( hkPackfileData* pkdata = bpkreader.getPackfileData() )
            {
               hkArray<hkVariant>& obj = bpkreader.getLoadedObjects();
               for ( int i =0,n=obj.getSize(); i<n; ++i)
               {
                  hkVariant& value = obj[i];
                  if ( value.m_class->hasVtable() )
                     defaultTypeRegistry.finishLoadedObject(value.m_object, value.m_class->getName());
               }
               resource = pkdata;
               resource->addReference();
            }
            root = bpkreader.getTopLevelObject();
         }
         break;

      case hkSerializeUtil::FORMAT_PACKFILE_XML:
         {
            xpkreader.loadEntireFileWithRegistry(reader, &defaultRegistry);
            if ( hkPackfileData* pkdata = xpkreader.getPackfileData() )
            {
               hkArray<hkVariant>& obj = xpkreader.getLoadedObjects();
               for ( int i =0,n=obj.getSize(); i<n; ++i)
               {
                  hkVariant& value = obj[i];
                  if ( value.m_class->hasVtable() )
                     defaultTypeRegistry.finishLoadedObject(value.m_object, value.m_class->getName());
               }
               resource = pkdata;
               resource->addReference();
               root = xpkreader.getTopLevelObject();
            }
         }
         break;

      case hkSerializeUtil::FORMAT_TAGFILE_BINARY:
      case hkSerializeUtil::FORMAT_TAGFILE_XML:
      default:
         {
            hkSerializeUtil::ErrorDetails detailsOut;
            hkSerializeUtil::LoadOptions loadflags = hkSerializeUtil::LOAD_FAIL_IF_VERSIONING;
            resource = hkSerializeUtilLoad(reader, &detailsOut, &defaultRegistry, loadflags);
            root.m_object = resource->getContents<hkRootLevelContainer>();
            if (root.m_object != NULL)
               root.m_class = &((hkRootLevelContainer*)root.m_object)->staticClass();
         }
         break;
      }
   }
   return root.m_object != NULL ? HK_SUCCESS : HK_FAILURE;
}

hkResource* hkSerializeUtilLoad( hkStreamReader* stream
                                , hkSerializeUtil::ErrorDetails* detailsOut/*=HK_NULL*/
                                , const hkClassNameRegistry* classReg/*=HK_NULL*/
                                , hkSerializeUtil::LoadOptions options/*=hkSerializeUtil::LOAD_DEFAULT*/ )
{
   __try
   {
      return hkSerializeUtil::load(stream, detailsOut, options);
   }
   __except (EXCEPTION_EXECUTE_HANDLER)
   {
      if (detailsOut == NULL)
         detailsOut->id = hkSerializeUtil::ErrorDetails::ERRORID_LOAD_FAILED;
      return NULL;
   }
}

void get_all_files_by_extension(vector<stringPaths_t> * all_paths, const char *cCurrentPath, const char * find, bool recurse)
{
	vector<string> dirs;
	vector<stringPaths_t> files;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(cCurrentPath)) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir(dir)) != NULL) {
		  if (ent->d_type == DIR_TYPE && ent->d_namlen > 2)
		  {
			  dirs.push_back(ent->d_name);
			  printf("This is DIR: %s\n", ent->d_name);
		  } else if (ent->d_type == FILE_TYPE && ent->d_namlen > strlen(find))
		  {
			  stringstream ss;
			  for (unsigned int i = strlen(ent->d_name) - strlen(find); i < strlen(ent->d_name); ++i)
			  {
				  ss << ent->d_name[i];
			  }
			  string s;
			  ss >> s;
			  if (strcmp(s.c_str(), find) == 0)
			  {
				  stringPaths_t sp;
				  string basepath, orig, tmp;
				  basepath.append(cCurrentPath);
				  basepath.append("\\");
				  orig.append(basepath);
				  orig.append(ent->d_name);
				  tmp.append(ent->d_name);
				  sp.original = orig;
				  for (unsigned int j = 0; j < strlen(find); ++j)
				  {
						  tmp.pop_back();
				  }
				  if (strcmp(tmp.c_str(), "Skeleton") != 0 && strcmp(tmp.c_str(), "Skeleton-out") != 0)
				  {
					  string newer;
					  newer.append(basepath);
					  newer.append(tmp);
					  newer.append(".damnhavok");
					  sp.newer = newer;
				  
					  files.push_back(sp);
				  }
			  }
			  printf ("This is file: %s\n", ent->d_name);
		  } else {
			  //printf(">> not adding %s, type: %d\n", ent->d_name, ent->d_type);
		  }
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
	  printf("could not open %s\n", cCurrentPath);
	}
	
	if (!files.empty()) 
	{
		for (vector<stringPaths_t>::iterator it = files.begin(); it != files.end(); ++it)
		{
			printf("adding file %s\n", it->original.c_str());
			all_paths->push_back(*it);
		}
	}

	if (!dirs.empty() && recurse)
	{
		for (vector<string>::iterator it = dirs.begin(); it != dirs.end(); ++it)
		{
			string s;
			s.append(cCurrentPath);
			s.append("\\");
			s.append(*it);
			printf("rec-call with: %s\n", s.c_str());
			get_all_files_by_extension(all_paths, s.c_str(), find, true);
		}
	}
}