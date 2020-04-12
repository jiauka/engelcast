# 
# Copyright (C) 2017/2020 Engel www.engel.es
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dn1004s
PKG_VERSION:=0.01
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/dn1004s

include $(INCLUDE_DIR)/package.mk

define Package/dn1004s
  SECTION:=base
  CATEGORY:=Utilities
  TITLE:=Roomy Cast DN1004S
endef

define Package/hello/description
 This package contains scripts for the roomycast operation and API
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef


define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/dn1004s/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,dn1004s))
