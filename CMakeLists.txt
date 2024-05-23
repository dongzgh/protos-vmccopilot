cmake_minimum_required(VERSION 3.14)

# Define project.
project(vmcCopilot)

# Define CXX settings.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define Qt settings.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 COMPONENTS Widgets Network WebEngineWidgets REQUIRED)

# Define target
# - Set header/source/ui files.
file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")
file(GLOB UIS "*.ui")

# - Add library.
add_library(vmcCopilot SHARED
    ${HEADERS}
    ${SOURCES}
    ${UIS})

# - Build target.
target_compile_definitions(vmcCopilot PRIVATE VMCCOPILOT_DLL_LIB)
target_link_libraries(vmcCopilot PRIVATE Qt6::Widgets Qt6::Network Qt6::WebEngineWidgets)

# Define installation settings.
# - Configure nuget package specification.
configure_file(VMCCopilot.nuspec.in VMCCopilot.nuspec)

# - Set header files.
set_target_properties(vmcCopilot PROPERTIES
	PUBLIC_HEADER "${HEADERS}"
)

# - Install target.
install(TARGETS vmcCopilot
	RUNTIME DESTINATION "lib/${CMAKE_BUILD_TYPE}"
	LIBRARY DESTINATION "lib/${CMAKE_BUILD_TYPE}"
	ARCHIVE DESTINATION "lib/${CMAKE_BUILD_TYPE}"
	PUBLIC_HEADER DESTINATION include
	RESOURCE DESTINATION resources
)

# - Install pdb files.
install(FILES 
	$<TARGET_PDB_FILE:vmcCopilot>
	DESTINATION "lib/${CMAKE_BUILD_TYPE}"
)

# - Install msbuild files.
install(FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/build/VMC.Copilot.props
	${CMAKE_CURRENT_SOURCE_DIR}/build/VMC.Copilot.targets
	DESTINATION ${CMAKE_INSTALL_PREFIX}/build
)

# - Install nuspec files.
install(FILES 
	${CMAKE_CURRENT_BINARY_DIR}/VMCCopilot.nuspec
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)

# Create NuGet package.
add_custom_target(vmcPackage
	COMMAND nuget pack ${CMAKE_INSTALL_PREFIX}/VMCCopilot.nuspec -OutputDirectory ${CMAKE_INSTALL_PACKAGE_DIRECTORY} -Verbosity quiet -NoPackageAnalysis
	DEPENDS vmcCopilot
)

# Organize header/source/ui files into folders.
source_group("headers" FILES ${HEADERS})
source_group("sources" FILES ${SOURCES})
source_group("uis" FILES ${UIS})
