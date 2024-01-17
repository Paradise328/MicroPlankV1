DESCRIPTION = "cifX device driver for Hilscher netX devices"
HOMEPAGE = "http://www.hilscher.com"
#see ${WORKDIR}/V${PV}/driver/libcifx/LICENSE
LICENSE = "CLOSED"

inherit cmake

DEBIAN_NOAUTONAME_${PN} = "1"

PACKAGES =+ "${PN}-plugin-spm"
RDEPENDS_${PN} += "${@bb.utils.contains('PACKAGECONFIG', 'spm', '${PN}-plugin-spm', '', d)}"

SRC_URI = "file://V${PV}.tar.gz"

PACKAGECONFIG ?= "${@bb.utils.contains('MACHINE_FEATURES', 'pci', 'pci', '' ,d)} \
                  tun"

PACKAGECONFIG[pci] = ",-DDISABLE_PCI=ON,libpciaccess,libpciaccess uionetx"
PACKAGECONFIG[spm] = "-DHWIF=ON -DSPM_PLUGIN=ON"
PACKAGECONFIG[tun] = "-DVIRTETH=ON"

S = "${WORKDIR}/V${PV}/driver/libcifx/"

do_install_append() {
  templates="${WORKDIR}/V${PV}/driver/templates/"

  #bootloader
  cd "${WORKDIR}/V${PV}/driver/BSL"
  install -d -m 0775 "${D}/opt/cifx/deviceconfig/"
  install -m 444 NETX*  "${D}/opt/cifx/"

  if [ "${@bb.utils.contains('PACKAGECONFIG', 'pci', 'yes', 'no', d)}" = "yes" ]; then
    install -d ${D}${nonarch_base_libdir}/udev/rules.d/
    install "${templates}/udev/80-udev-netx.rules" ${D}${nonarch_base_libdir}/udev/rules.d/80-hilscher-netx.rules
  fi

  if [ "${@bb.utils.contains('PACKAGECONFIG', 'tun', 'yes', 'no', d)}" = "yes" ]; then
    install -d ${D}${nonarch_base_libdir}/udev/rules.d/
    install "${templates}/udev/80-udev-cifxeth.rules" ${D}${nonarch_base_libdir}/udev/rules.d/80-hilscher-cifxeth.rules

    install -d "${D}/etc/init.d/"
    install -m 0744 "${templates}/udev/cifxeth" ${D}/etc/init.d/
  fi

  if [ "${@bb.utils.contains('PACKAGECONFIG', 'spm', 'yes', 'no', d)}" = "yes" ]; then
    install -d ${D}/opt/cifx/plugins/netx-spm/
    install -m 0744 "${templates}/plugins/netx-spm/config0" ${D}/opt/cifx/plugins/netx-spm/config0
  fi
  chmod 0775 ${D}/opt/cifx
}

FILES_${PN} += "/usr/lib/ \
                /opt/cifx/ \
                ${nonarch_base_libdir}/udev/rules.d/80-hilscher-netx.rules \
                ${nonarch_base_libdir}/udev/rules.d/80-hilscher-cifxeth.rules"

FILES_${PN}-plugin-spm += "/opt/cifx/plugins/"
