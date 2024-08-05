nvcc -arch=sm_75 -O3 tetration_cuda.cu -o .\tetration_cuda.exe
del *.lib *.exp
tetration_cuda.exe