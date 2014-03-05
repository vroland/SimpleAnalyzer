prefix=$(DESTDIR)/usr/local

# All of the sources participating in the build are defined here

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: simpleanalyzer

# Tool invocations
simpleanalyzer: $(OBJS) $(USER_OBJS)
	make --directory=csvtosd/Debug
	make --directory=odisitosd/Debug
	make --directory=mergetsd/build
	make --directory=simpleanalyzer-gui/Debug

# Other Targets
clean:
	make --directory=csvtosd/Debug clean
	make --directory=odisitosd/Debug clean
	make --directory=mergetsd/build clean
	make --directory=simpleanalyzer-gui/Debug clean
install: all
	@echo $(prefix)
	@echo $(datadir)
	mkdir -p $(prefix)/bin
	mkdir -p $(prefix)/share/simpleanalyzer/
	mkdir -p $(prefix)/share/simpleanalyzer/icons
	mkdir -p $(prefix)/share/applications/
	
	install --mode=755 csvtosd/Debug/csvtosd $(prefix)/bin
	install --mode=755 csvtosd/Debug/csvtosd.conf $(prefix)/share/simpleanalyzer/
	
	install --mode=755 odisitosd/Debug/odisitosd $(prefix)/bin
	install --mode=755 odisitosd/Debug/odisitosd.conf $(prefix)/share/simpleanalyzer/
	
	install --mode=755 mergetsd/build/mergetsd $(prefix)/bin
	
	install --mode=755 simpleanalyzer-gui/Debug/simpleanalyzer-gui $(prefix)/bin
	install --mode=755 simpleanalyzer-gui/Debug/icons/analyze_point.png $(prefix)/share/simpleanalyzer/icons
	install --mode=755 simpleanalyzer-gui/Debug/icons/prgm-icon.png $(prefix)/share/simpleanalyzer/icons
	install --mode=755 simpleanalyzer-gui/Debug/icons/cut_icon.png $(prefix)/share/simpleanalyzer/icons
	install --mode=755 simpleanalyzer-gui/Debug/icons/obj_icon.png $(prefix)/share/simpleanalyzer/icons
	install --mode=755 simpleanalyzer-gui/Debug/icons/overview_icon.png $(prefix)/share/simpleanalyzer/icons
	install --mode=755 simpleanalyzer-gui/Debug/icons/sd_icon.png $(prefix)/share/simpleanalyzer/icons
	install --mode=755 simpleanalyzer-gui/Debug/icons/tsd_icon.png $(prefix)/share/simpleanalyzer/icons
	install --mode=755 simpleanalyzer-gui/Debug/icons/cut_icon.png $(prefix)/share/simpleanalyzer/icons
	install --mode=755 simpleanalyzer-gui/Debug/simpleanalyzer-man.pdf $(prefix)/share/simpleanalyzer/
	
	install --mode=755 simpleanalyzer-gui.desktop $(prefix)/share/applications/
uninstall: all
	rm -rf $(prefix)/share/simpleanalyzer/
	
	rm -f $(prefix)/bin/odisitosd
	rm -f $(prefix)/bin/csvtosd
	rm -f $(prefix)/bin/mergetsd
	rm -f $(prefix)/bin/simpleanalyzer-gui
	rm -f $(prefix)/share/applications/simpleanalyzer-gui.desktop
distclean: clean
.PHONY: install 

