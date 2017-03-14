# Telephony permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml

# RIL properties
PRODUCT_PROPERTY_OVERRIDES += \
    rild.libargs=-d[SPACE]/dev/smd0 \
    rild.libpath=/system/lib/libril-qc-qmi-1.so \
    ro.telephony.ril_class=NokiaRIL

# Radio properties
PRODUCT_PROPERTY_OVERRIDES += \
    persist.radio.apm_sim_not_pwdn=1 \
    ril.subscription.types=NV,RUIM \
    telephony.lteOnCdmaDevice=0

# Telephony properties
PRODUCT_PROPERTY_OVERRIDES += \
    ro.telephony.call_ring.multiple=0
