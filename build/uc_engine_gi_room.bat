@echo off


echo ^<?xml version="1.0" encoding="utf-8"?^> > uc_engine_gi_room_h.msbuild
echo ^<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003"^> >> uc_engine_gi_room_h.msbuild
echo ^<ItemGroup^> >> uc_engine_gi_room_h.msbuild

ucdev_build_file_generator_r.exe --input ..\src\uc_engine_gi_room\ --mode h >> uc_engine_gi_room_h.msbuild
ucdev_build_file_generator_r.exe --input ..\include\uc_public\ --mode h >> uc_engine_gi_room_h.msbuild

echo ^</ItemGroup^> >> uc_engine_gi_room_h.msbuild
echo ^</Project^> >> uc_engine_gi_room_h.msbuild



echo ^<?xml version="1.0" encoding="utf-8"?^> > uc_engine_gi_room_cpp.msbuild
echo ^<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003"^> >> uc_engine_gi_room_cpp.msbuild
echo ^<ItemGroup^> >> uc_engine_gi_room_cpp.msbuild

ucdev_build_file_generator_r.exe --input ..\src\uc_engine_gi_room\ --mode cpp >> uc_engine_gi_room_cpp.msbuild

echo ^</ItemGroup^> >> uc_engine_gi_room_cpp.msbuild
echo ^</Project^> >> uc_engine_gi_room_cpp.msbuild

echo ^<?xml version="1.0" encoding="utf-8"?^> > uc_engine_gi_room_hlsl.msbuild
echo ^<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003"^> >> uc_engine_gi_room_hlsl.msbuild
echo ^<ItemGroup^> >> uc_engine_gi_room_hlsl.msbuild

ucdev_build_file_generator_r.exe --input ..\src\uc_engine_gi_room\shaders\ --mode hlsl >> uc_engine_gi_room_hlsl.msbuild
ucdev_build_file_generator_r.exe --input ..\include\uc_public\gpu\ --mode hlsl >> uc_engine_gi_room_hlsl.msbuild


echo ^</ItemGroup^> >> uc_engine_gi_room_hlsl.msbuild
echo ^</Project^> >> uc_engine_gi_room_hlsl.msbuild


echo ^<?xml version="1.0" encoding="utf-8"?^> > uc_engine_gi_room_pso.msbuild
echo ^<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003"^> >> uc_engine_gi_room_pso.msbuild
echo ^<ItemGroup^> >> uc_engine_gi_room_pso.msbuild

ucdev_build_file_generator_r.exe --input ..\src\uc_engine_gi_room\shaders\ --mode pso >> uc_engine_gi_room_pso.msbuild

echo ^</ItemGroup^> >> uc_engine_gi_room_pso.msbuild
echo ^</Project^> >> uc_engine_gi_room_pso.msbuild




