@echo off
for /D %%G in ("./VM*") DO ( 

    echo Stopping %%G : ...
    cd %~dp0%%G
    vagrant halt -f
    vagrant destroy -f
    echo Done

)