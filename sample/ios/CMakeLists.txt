message("Building IOS Sample")

macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
	set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro (set_xcode_property)

set(ezmidi_ios_sample_source
	main.m
	AppDelegate.h
	AppDelegate.m
	ViewController.h
	ViewController.m
)

set(ezmidi_ios_sample_resources
	LaunchScreen.storyboard
	Main.storyboard
)

add_executable(ezmidi_ios_sample MACOSX_BUNDLE 
	${ezmidi_ios_sample_source}
	${ezmidi_ios_sample_resources}
)

set(APP_BUNDLE_IDENTIFIER "ezmidi.ios.sample")
set(APP_FRAMEWORK_IDENTIFIER "ezmidi.ios.sample.framework")

set_xcode_property(ezmidi_ios_sample PRODUCT_BUNDLE_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})
set(MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})
set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_BUNDLE_IDENTIFIER})

target_compile_options(ezmidi_ios_sample PUBLIC "-fobjc-arc")
set_target_properties(ezmidi_ios_sample PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)

set_target_properties(ezmidi_ios_sample PROPERTIES
  MACOSX_BUNDLE TRUE
  MACOSX_FRAMEWORK_IDENTIFIER ${APP_FRAMEWORK_IDENTIFIER}
  RESOURCE "${ezmidi_ios_sample_resources}"
)

target_link_libraries(ezmidi_ios_sample PUBLIC ezmidi)
target_link_libraries(ezmidi_ios_sample PUBLIC "-framework UIKit")