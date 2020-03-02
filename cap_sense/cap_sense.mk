

# The name of the project component
NAME := app_emlib_cap_sense


# Enable/disable automatic generation of static function prototypes in project source code
$(NAME)_AUTO_PROTOTYPE := 1

# List of files to include in the project build. Paths relative to the project's directory
$(NAME)_SOURCES := main.c


# Include the webapp resource files
$(NAME)_REQUIRED_COMPONENTS := resources.gecko_os.webapp