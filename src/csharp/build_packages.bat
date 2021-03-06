@rem Builds gRPC NuGet packages

@rem Current package versions
set VERSION=0.12.0
set CORE_VERSION=0.12.0
set PROTOBUF_VERSION=3.0.0-beta2

@rem Packages that depend on prerelease packages (like Google.Protobuf) need to have prerelease suffix as well.
set VERSION_WITH_BETA=%VERSION%-beta

@rem Adjust the location of nuget.exe
set NUGET=C:\nuget\nuget.exe

@rem Collect the artifacts built by the previous build step if running on Jenkins
@rem TODO(jtattermusch): is there a better way to do this?
xcopy /Y /I ..\..\architecture=x86,language=csharp,platform=windows\artifacts\* grpc.native.csharp\windows_x86\
xcopy /Y /I ..\..\architecture=x64,language=csharp,platform=windows\artifacts\* grpc.native.csharp\windows_x64\
xcopy /Y /I ..\..\architecture=x86,language=csharp,platform=linux\artifacts\* grpc.native.csharp\linux_x86\
xcopy /Y /I ..\..\architecture=x64,language=csharp,platform=linux\artifacts\* grpc.native.csharp\linux_x64\
xcopy /Y /I ..\..\architecture=x86,language=csharp,platform=macos\artifacts\* grpc.native.csharp\macosx_x86\
xcopy /Y /I ..\..\architecture=x64,language=csharp,platform=macos\artifacts\* grpc.native.csharp\macosx_x64\

@rem Fetch all dependencies
%NUGET% restore ..\..\vsprojects\grpc_csharp_ext.sln || goto :error
%NUGET% restore Grpc.sln || goto :error

setlocal

@call "%VS120COMNTOOLS%\..\..\vc\vcvarsall.bat" x86

@rem We won't use the native libraries from this step, but without this Grpc.sln will fail.  
msbuild ..\..\vsprojects\grpc_csharp_ext.sln /p:Configuration=Release /p:PlatformToolset=v120 || goto :error

msbuild Grpc.sln /p:Configuration=ReleaseSigned || goto :error

endlocal

@rem TODO(jtattermusch): re-enable protoc plugin building
@rem @call ..\..\vsprojects\build_plugins.bat || goto :error

%NUGET% pack grpc.native.csharp\grpc.native.csharp.nuspec -Version %CORE_VERSION% || goto :error
%NUGET% pack Grpc.Auth\Grpc.Auth.nuspec -Symbols -Version %VERSION% || goto :error
%NUGET% pack Grpc.Core\Grpc.Core.nuspec -Symbols -Version %VERSION% -Properties GrpcNativeCsharpVersion=%CORE_VERSION% || goto :error
%NUGET% pack Grpc.HealthCheck\Grpc.HealthCheck.nuspec -Symbols -Version %VERSION_WITH_BETA% -Properties ProtobufVersion=%PROTOBUF_VERSION% || goto :error
%NUGET% pack Grpc.nuspec -Version %VERSION% || goto :error

@rem TODO(jtattermusch): re-enable building Grpc.Tools package
@rem %NUGET% pack Grpc.Tools.nuspec -Version %VERSION% || goto :error

@rem copy resulting nuget packages to artifacts directory
xcopy /Y /I *.nupkg ..\..\artifacts\

goto :EOF

:error
echo Failed!
exit /b %errorlevel%
