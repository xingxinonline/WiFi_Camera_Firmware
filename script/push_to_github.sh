#!/bin/bash

OUT_FOLDER="WiFi_Camera"

read -p "plese input comment:" comment

if [ -n "$comment" ]; then 
    echo ">>> Kill IAR output files"
    rm -r "../EWARM/$OUT_FOLDER"

    echo ">>> git pull from server"
    git pull origin master

    echo ">>> git add files"
    git add ../*
    
    echo ">>> git commit files"
    git commit -m "$comment"
    
    echo ">>> git push to server "
    git push origin master
    
else
    echo "you can't push without comment"
fi


