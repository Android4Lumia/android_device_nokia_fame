# GPS permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml

# GPS packages
PRODUCT_PACKAGES += \
    gps.msm8960 \
    gps.conf \
    sap.conf \
    izat.conf
