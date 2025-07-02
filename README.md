
## Kurulum ve Derleme Adımları

Aşağıdaki adımları sırasıyla uygulayarak projeyi derleyebilirsiniz:

1. Gerekli bağımlılıkları yükleyin:
    ```sh
    conan install . --build=missing -pr:b=default -pr:h=./profiles/pizero2w
    ```

2. `build` dizinine geçin ve ortam değişkenlerini ayarlayın:
    ```sh
    cd build
    source Release/generators/conanbuild.sh
    ```

3. CMake ile projeyi yapılandırın ve derleyin:
    ```sh
    cmake .. -DCMAKE_TOOLCHAIN_FILE=Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
    cmake --build .
    ```

4. Ortam değişkenlerini devre dışı bırakın:
    ```sh
    source Release/generators/deactivate_conanbuild.sh
    ```

5. `p2p_client` dosyasını kullanıma hazır hale getirin.

