SUMMARY = "cifX device driver for Hilscher netX devices - kernel mode driver"
HOMEPAGE = "www.hilscher.com"
LICENSE = "GPLv2"

LIC_FILES_CHKSUM = "file://uio_netx.c;endline=12;md5=95fc4e2e758291d082694859311f7cea"

SRC_URI = "file://V${PV}.tar.gz"

S = "${WORKDIR}/V${PV}/driver/uio_netx"

EXTRA_OEMAKE = "${@bb.utils.contains('DISTRO_FEATURES', 'grsecurity', 'DISABLE_PAX_PLUGINS=y', '', d)} \
                KDIR=${STAGING_KERNEL_DIR} \
                TMPSYMVERS=${KBUILD_EXTRA_SYMBOLS}"

inherit module
