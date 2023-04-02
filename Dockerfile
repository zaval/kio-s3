FROM kdeneon/plasma:user
USER root
RUN apt-get update && apt-get install -y cmake cmake-extras build-essential zlib1g-dev libcurl4 libcurlpp-dev libcrypto++-dev libssl-dev libcurl4-openssl-dev qtbase5-dev qtdeclarative5-dev libqt5svg5-dev extra-cmake-modules libkf5kio-dev libkf5i18n-dev libkf5kcmutils-dev  libkf5declarative-dev libkf5config-dev libkf5wallet-dev libkf5notifications-dev gettext git
RUN git clone --recurse-submodules --depth 1 --branch 1.11.43 https://github.com/aws/aws-sdk-cpp.git
RUN cd aws-sdk-cpp && mkdir "build" && cd build && \
    cmake -DBUILD_ONLY=s3 -DENABLE_TESTING=OFF -DCMAKE_INSTALL_PREFIX=/home/neon/aws-sdk .. && \
    make -j && make install
RUN cd /home/neon && rm -rf /home/neon/aws-sdk-cpp
