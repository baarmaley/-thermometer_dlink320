#=====================================
# OpenWrt Makefile
#=====================================

include $(TOPDIR)/rules.mk

#LIBS      += `pkg-config libudev --libs`
#LIBS  ?= "/home/barmaley/openwrt/trunk/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/lib/libusb-1.0.a"
#LIBS += "/home/barmaley/openwrt/trunk/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/lib/libiconv-full/lib/libiconv.a"
#LIBS  ?= -L"$(STAGING_DIR)/lib/" -L$(STAGING_DIR)/usr/lib -ludev -luClibc++ -lc -lm -lgcc
LFLAGS ?= -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib
LIBS ?= -lc -lm -lgcc -lstdc++ -ludev

#INCLUDES ?= /home/barmaley/openwrt/trunk/staging_dir/target-mipsel_24kes+dsp_uClibc_0.9.33.2/usr/include
#TARGET_CFLAGS += -I"/home/barmaley/openwrt/trunk/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/include"
#TARGET_CFLAGS += -I"/home/barmaley/openwrt/trunk/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/lib/libiconv-full/include"

PKG_NAME:=relay
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_BUILD_DIR:= $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk 


define Package/relay
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Relay Comtrol
	DEPENDS:= +udev +libstdcpp +libc
endef

define Package/relay/description
	Relay Comtrol
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef


define Build/Compile

	$(TARGET_CC) $(TARGET_CFLAGS)  -c -o $(PKG_BUILD_DIR)/hid.o $(PKG_BUILD_DIR)/hid.c
	$(TARGET_CC) $(TARGET_CFLAGS)  -c  -o $(PKG_BUILD_DIR)/relaycontrol.o $(PKG_BUILD_DIR)/relaycontrol.cpp
	$(TARGET_CC) $(TARGET_CFLAGS)  -c  -o $(PKG_BUILD_DIR)/relay.o $(PKG_BUILD_DIR)/relay.cpp
	$(TARGET_CC) $(TARGET_CFLAGS) $(LFLAGS)  $(PKG_BUILD_DIR)/relay.o $(PKG_BUILD_DIR)/hid.o $(PKG_BUILD_DIR)/relaycontrol.o  -o $(PKG_BUILD_DIR)/relay $(LIBS)
endef

define Package/relay/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/relay $(1)/bin/
	chmod 777 $(1)/bin/relay
endef

$(eval $(call BuildPackage,relay))
