KIO AWS S3 Worker
----------------------

### Build instructions

```
mkdir build
cd build
cmake \
    -DKDE_INSTALL_PLUGINDIR=`kf5-config --qt-plugins` \
    -DKDE_INSTALL_KSERVICESDIR=`kf5-config --install services` \
    -DKDE_INSTALL_LOGGINGCATEGORIESDIR=`kf5-config --install data`qlogging-categories5 \
    -DCMAKE_BUILD_TYPE=Release  ..
make
make install
```

After this you can test the new protocol with:  
`kioclient5 'ls' 's3:///'`  
`kioclient5 'cat' 's3:///Item A'`

You can also explore the new protocol with dolphin or konqueror:  
`dolphin s3:///`


### Related documentation

Find the documentation of `KIO::WorkerBase` for the API to implement at
https://api.kde.org/frameworks/kio/html/classKIO_1_1WorkerBase.html

Learn about debugging your new KIO worker at
https://community.kde.org/Guidelines_and_HOWTOs/Debugging/Debugging_IOSlaves
