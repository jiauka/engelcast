include $(TOPDIR)/rules.mk

PKG_NAME:=roomy-repeater
PKG_VERSION:=1.9
PKG_RELEASE:=1

PKG_LICENSE:=GPL

include $(INCLUDE_DIR)/package.mk

define Package/roomy-repeater
  SECTION:=net
  CATEGORY:=Network
  TITLE:=roomy-repeater is a Multicast DNS repeater for Linux
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef
define Build/Compile
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/roomy-repeater $(PKG_BUILD_DIR)/roomy-repeater.c
endef

define Package/roomy-repeater/install

	$(INSTALL_DIR) -v $(1)/usr/sbin
	$(INSTALL_BIN) -v $(PKG_BUILD_DIR)/roomy-repeater $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/roomy-repeater.init.sh $(1)/etc/init.d/roomy-repeater
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,roomy-repeater))


