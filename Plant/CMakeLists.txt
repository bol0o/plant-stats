cmake_minimum_required(VERSION 3.16)

project(Plant VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Szukamy pakietów Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Bluetooth Charts)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    blemanager.cpp
    blemanager.h
)

# Tworzymy aplikację (uproszczone dla Qt6)
qt_add_executable(Plant
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# Linkowanie bibliotek
target_link_libraries(Plant PRIVATE
    Qt6::Widgets
    Qt6::Bluetooth
    Qt6::Charts
)

# Konfiguracja macOS (Uprawnienia Bluetooth)
if(APPLE)
    set(MACOS_INFO_PLIST_CONTENT "
    <?xml version=\"1.0\" encoding=\"UTF-8\"?>
    <!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">
    <plist version=\"1.0\">
    <dict>
        <key>NSBluetoothAlwaysUsageDescription</key>
        <string>Aplikacja potrzebuje dostępu do Bluetooth, aby odbierać dane z sensora kwiatka.</string>
        <key>NSBluetoothPeripheralUsageDescription</key>
        <string>Aplikacja potrzebuje dostępu do Bluetooth, aby łączyć się z XIAO-C6.</string>
        <key>CFBundleIdentifier</key>
        <string>com.twojanazwa.plant</string>
        <key>CFBundleExecutable</key>
        <string>Plant</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
    </dict>
    </plist>
    ")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/Info.plist" "${MACOS_INFO_PLIST_CONTENT}")
endif()

set_target_properties(Plant PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(Plant)