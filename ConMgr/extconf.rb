require 'mkmf'

PLATFORM_IS_OSX = (Object::RUBY_PLATFORM =~ /darwin/i) ? true : false

# Name the extension.
extension_name = 'ConnectionManager'

if PLATFORM_IS_OSX

	# Must be compiled into flat namespace. Otherwise SketchUp Bugsplats.
	CONFIG['LDSHARED'] = 'cc -dynamic -bundle -undefined suppress -flat_namespace'

else
	dir_config("curl")
	
	# Create the header file.
	create_header('ConnectionManager.h')
end

# Make sure the cURL library is installed.
have_library("curl")
# Create the Makefile.
create_makefile(extension_name)


