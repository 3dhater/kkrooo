// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"

#include <cstdlib>
#include <cctype>
#include <functional>


#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifdef KK_PLATFORM_WINDOWS
#pragma comment(lib, "..\\..\\3rd\\assimp\\lib\\x64\\assimp.lib")
#endif

#ifdef _WIN32
#include <Windows.h>
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch( ul_reason_for_call )
	{
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif

extern "C" 
{

	KK_API int KK_C_DECL kkPlugin_onImportMesh( kkPluginCommonInterface* cInterface, const char16_t* file_path )
	{
		int result = 1; //good
		
		auto geometry_creator = cInterface->GetGeometryCreator();
		if( cInterface->FSFileExist( file_path ) )
		{
			auto stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
			aiAttachLogStream(&stream);
			
			kkString str = file_path;
			kkStringA stra;
			util::string_UTF16_to_UTF8(str,stra);

			auto scene = aiImportFile(  stra.data(), 0 );

			if( !scene )
			{
				return 0;
			}


			std::function<void(aiNode*,aiMatrix4x4*)> read3Ds;

			read3Ds = [&](aiNode* node, aiMatrix4x4* parentMatrix)
			{
				aiMatrix4x4 currentMatrix = *parentMatrix * node->mTransformation;



				//aiTransposeMatrix4(&m);
				
				for( u32 i = 0; i < node->mNumMeshes; ++i) 
				{
					const C_STRUCT aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

					geometry_creator->setName( kkString( mesh->mName.data ).data() );
					
					geometry_creator->beginModel();


					u32 pos_index = 0;

					for( u32 t = 0; t < mesh->mNumFaces; ++t)
					{
						const C_STRUCT aiFace* face = &mesh->mFaces[t];

						geometry_creator->beginPolygon();

						for( u32 in = 0; in < face->mNumIndices; in++)
						{
							int index = face->mIndices[in];

							geometry_creator->beginVertex( pos_index++ );

							auto v = currentMatrix * mesh->mVertices[index];
							geometry_creator->setPosition( v.x, -v.z, v.y );

							if(mesh->mNormals != NULL)
							{
								geometry_creator->setNormal( 
									mesh->mNormals[index].x, 
									mesh->mNormals[index].z,
									mesh->mNormals[index].y
								);
							}

							geometry_creator->endVertex();
						}

						geometry_creator->endPolygon( true, false, false );
					}

					geometry_creator->generateNormals(false);
					geometry_creator->endModel();
				}

				for( u32 n = 0; n < node->mNumChildren; ++n)
				{
					read3Ds(node->mChildren[n], &currentMatrix);
				}
			};

			read3Ds(scene->mRootNode,&scene->mRootNode->mTransformation);

			aiReleaseImport(scene);
			aiDetachAllLogStreams();

		}

		return result;
	}

}