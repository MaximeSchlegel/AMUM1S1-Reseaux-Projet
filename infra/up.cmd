@echo off
for /D %%G in ("./VM*") DO ( 

    echo Launching %%G : ...
    cd %~dp0%%G
    vagrant up
    echo     Done

)