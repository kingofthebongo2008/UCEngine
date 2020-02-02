msbuild src/tools/tools.sln /t:Rebuild /p:Configuration=release /p:Platform=x64 /verbosity:normal
msbuild src/tools/tools.sln /t:Rebuild /p:Configuration=debug /p:Platform=x64 /verbosity:normal
msbuild build/ucdev.sln /t:rebuild /p:Configuration=release /p:Platform=x64 /verbosity:normal
msbuild build/ucdev.sln /t:rebuild /p:Configuration=release /p:Platform=x64 /verbosity:normal
rem msbuild build/uc_engine.sln /t:Build /p:Configuration=release /p:Platform=x64 /verbosity:normal
