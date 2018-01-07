@echo off

ucdev_cpp_source_generator_r.exe --input ..\src\uc_engine\ --mode cpp > uc_engine_cpp1.txt
ucdev_cpp_source_generator_r.exe --input ..\src\uc_dev\ --mode cpp    > uc_engine_cpp2.txt

ucdev_cpp_source_generator_r.exe --input ..\src\uc_engine\ --mode h > uc_engine_header1.txt
ucdev_cpp_source_generator_r.exe --input ..\src\uc_dev\ --mode h > uc_engine_header2.txt
ucdev_cpp_source_generator_r.exe --input ..\include\uc_public\ --mode h > uc_engine_header3.txt

copy uc_engine_header1.txt + uc_engine_header2.txt+uc_engine_header3.txt uc_engine_header.txt
copy uc_engine_cpp1.txt + uc_engine_cpp2.txt uc_engine_cpp.txt

del /Q uc_engine_header1.txt
del /Q uc_engine_header2.txt
del /Q uc_engine_header3.txt

del /Q uc_engine_cpp1.txt
del /Q uc_engine_cpp2.txt



