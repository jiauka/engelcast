# 
# Copyright (C) 2017/2020 Engel www.engel.es
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=engelcast
PKG_VERSION:=0.01
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/engelcast

include $(INCLUDE_DIR)/package.mk

define Package/engelcast
  SECTION:=base
  CATEGORY:=Utilities
  TITLE:=EngelCast service
endef

define Package/engelcast/description
 This package contains scripts for the roomycast operation and API
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef


define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/engelcast/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,engelcast))
