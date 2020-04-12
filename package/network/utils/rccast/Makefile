# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rccast
PKG_VERSION:=0.01
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/rccast

include $(INCLUDE_DIR)/package.mk

define Package/rccast
  SECTION:=network
  CATEGORY:=Network
  TITLE:=ROOMYCAST CC handler
  DEPENDS:=+libopenssl +jansson 
endef

define Package/rccast/description
 This package contains an utility for RoomyCast CC handling
 based switches.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/rccast/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rccast $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,rccast))
