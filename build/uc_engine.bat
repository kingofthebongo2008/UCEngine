@echo off


echo ^<?xml version="1.0" encoding="utf-8"?^> > uc_engine_h.msbuild
echo ^<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003"^> >> uc_engine_h.msbuild
echo ^<ItemGroup^> >> uc_engine_h.msbuild

ucdev_cpp_source_generator_r.exe --input ..\src\uc_engine\ --mode h >> uc_engine_h.msbuild
ucdev_cpp_source_generator_r.exe --input ..\src\uc_dev\ --mode h >> uc_engine_h.msbuild
ucdev_cpp_source_generator_r.exe --input ..\include\uc_public\ --mode h >> uc_engine_h.msbuild

echo ^</ItemGroup^> >> uc_engine_h.msbuild
echo ^</Project^> >> uc_engine_h.msbuild



echo ^<?xml version="1.0" encoding="utf-8"?^> > uc_engine_cpp.msbuild
echo ^<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003"^> >> uc_engine_cpp.msbuild
echo ^<ItemGroup^> >> uc_engine_cpp.msbuild

ucdev_cpp_source_generator_r.exe --input ..\src\uc_engine\ --mode cpp >> uc_engine_cpp.msbuild
ucdev_cpp_source_generator_r.exe --input ..\src\uc_dev\ --mode cpp    >> uc_engine_cpp.msbuild

echo ^</ItemGroup^> >> uc_engine_cpp.msbuild
echo ^</Project^> >> uc_engine_cpp.msbuild



