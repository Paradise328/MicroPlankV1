SUMMARY = "cifX device driver example applications for Hilscher netX devices"
HOMEPAGE = "www.hilscher.com"
LICENSE = "CLOSED"

DEPENDS = "libcifx"

SRC_URI = "file://V${PV}.tar.gz"

inherit cmake

S = "${WORKDIR}/V${PV}/examples"

FILES_${PN} = "/opt/cifx/demo"

EXTRA_OECMAKE = "-DCMAKE_INSTALL_PREFIX=/opt/cifx/demo/"

do_install_append() {
  mv "${D}/opt/cifx/demo/bin/"* "${D}/opt/cifx/demo"
  rm -rf "${D}/opt/cifx/demo/bin"

  chmod 0775 ${D}/opt/cifx
}
