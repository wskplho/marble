PROJECT(NavigationFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( navigation_SRCS NavigationFloatItem.cpp ArrowDiscWidget.cpp NavigationButton.cpp NavigationSlider.cpp )
set (navigation_UI navigation.ui)

qt_add_resources(navigation_SRCS navigation.qrc)
qt_wrap_ui(navigation_SRCS ${navigation_UI})

marble_add_plugin( NavigationFloatItem ${navigation_SRCS} )
