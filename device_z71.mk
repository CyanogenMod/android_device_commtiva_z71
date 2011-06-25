## This device is the Commtiva Z1, and all its variants, which include:
# Optimus Boston
# Orange Boston
# Gigabyte Gsmart G1305
# Motorola XT502
# Apanda A60
# Vibo A688
# Chinavision Excalibur
# Muchtel A1
# Wellcom A88
# Cincinnati Bell Blaze
# Spice Mi-300
# Nexian A-890 Journey
#

# The gps config appropriate for this device
$(call inherit-product, device/common/gps/gps_us_supl.mk)

DEVICE_PACKAGE_OVERLAYS += device/commtiva/z71/overlay

# HAL libs and other system binaries
PRODUCT_PACKAGES += \
    hwprops \
    abtfilt \
    gps.z71 \
    lights.z71 \
    copybit.z71 \
    gralloc.z71 \
    libOmxCore

# Extra apps
PRODUCT_PACKAGES += \
	Torch

ifeq ($(TARGET_PREBUILT_KERNEL),)
    LOCAL_KERNEL := device/commtiva/z71/kernel
else
    LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

PRODUCT_COPY_FILES += \
    $(LOCAL_KERNEL):kernel


# Live wallpaper packages
PRODUCT_PACKAGES += \
    LiveWallpapers \
    LiveWallpapersPicker \
    MagicSmokeWallpapers \
    VisualizationWallpapers \
    librs_jni

# Publish that we support the live wallpaper feature.
PRODUCT_COPY_FILES += \
    packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:/system/etc/permissions/android.software.live_wallpaper.xml

# Board-specific init
PRODUCT_COPY_FILES += \
    device/commtiva/z71/ueventd.qct.rc:root/ueventd.qct.rc \
    device/commtiva/z71/init.qcom.rc:root/init.qcom.rc

## RIL related stuff
PRODUCT_COPY_FILES += \
    device/commtiva/z71/spn-conf.xml:system/etc/spn-conf.xml \
    vendor/commtiva/z71/proprietary/bin/qmuxd:system/bin/qmuxd \
    vendor/commtiva/z71/proprietary/lib/liboncrpc.so:system/lib/liboncrpc.so \
    vendor/commtiva/z71/proprietary/lib/libmmgsdilib.so:system/lib/libmmgsdilib.so \
    vendor/commtiva/z71/proprietary/lib/libgsdi_exp.so:system/lib/libgsdi_exp.so \
    vendor/commtiva/z71/proprietary/lib/libgstk_exp.so:system/lib/libgstk_exp.so \
    vendor/commtiva/z71/proprietary/lib/libwms.so:system/lib/libwms.so \
    vendor/commtiva/z71/proprietary/lib/libnv.so:system/lib/libnv.so \
    vendor/commtiva/z71/proprietary/lib/libwmsts.so:system/lib/libwmsts.so \
    vendor/commtiva/z71/proprietary/lib/libdss.so:system/lib/libdss.so \
    vendor/commtiva/z71/proprietary/lib/libqmi.so:system/lib/libqmi.so \
    vendor/commtiva/z71/proprietary/lib/libdiag.so:system/lib/libdiag.so \
    vendor/commtiva/z71/proprietary/lib/libpbmlib.so:system/lib/libpbmlib.so \
    vendor/commtiva/z71/proprietary/lib/libauth.so:system/lib/libauth.so \
    vendor/commtiva/z71/proprietary/lib/liboem_rapi.so:system/lib/liboem_rapi.so \
    vendor/commtiva/z71/proprietary/lib/libdsm.so:system/lib/libdsm.so \
    vendor/commtiva/z71/proprietary/lib/libqueue.so:system/lib/libqueue.so \
    vendor/commtiva/z71/proprietary/lib/libcm.so:system/lib/libcm.so \
    vendor/commtiva/z71/proprietary/lib/libdll.so:system/lib/libdll.so \
    vendor/commtiva/z71/proprietary/lib/libril-qc-1.so:system/lib/libril-qc-1.so \
    vendor/commtiva/z71/proprietary/lib/libril-qcril-hook-oem.so:system/lib/libril-qcril-hook-oem.so

## OMX proprietaries
PRODUCT_COPY_FILES += \
    vendor/commtiva/z71/proprietary/lib/libmm-adspsvc.so:system/lib/libmm-adspsvc.so \
    vendor/commtiva/z71/proprietary/lib/libOmxAacDec.so:system/lib/libOmxAacDec.so \
    vendor/commtiva/z71/proprietary/lib/libOmxAmrRtpDec.so:system/lib/libOmxAmrRtpDec.so \
    vendor/commtiva/z71/proprietary/lib/libOmxH264Dec.so:system/lib/libOmxH264Dec.so \
    vendor/commtiva/z71/proprietary/lib/libOmxQcelpDec.so:system/lib/libOmxQcelpDec.so \
    vendor/commtiva/z71/proprietary/lib/libOmxAacEnc.so:system/lib/libOmxAacEnc.so \
    vendor/commtiva/z71/proprietary/lib/libOmxAmrwbDec.so:system/lib/libOmxAmrwbDec.so \
    vendor/commtiva/z71/proprietary/lib/libOmxMp3Dec.so:system/lib/libOmxMp3Dec.so \
    vendor/commtiva/z71/proprietary/lib/libOmxVidEnc.so:system/lib/libOmxVidEnc.so \
    vendor/commtiva/z71/proprietary/lib/libOmxAmrDec.so:system/lib/libOmxAmrDec.so \
    vendor/commtiva/z71/proprietary/lib/libOmxEvrcDec.so:system/lib/libOmxEvrcDec.so \
    vendor/commtiva/z71/proprietary/lib/libOmxMpeg4Dec.so:system/lib/libOmxMpeg4Dec.so \
    vendor/commtiva/z71/proprietary/lib/libOmxWmaDec.so:system/lib/libOmxWmaDec.so \
    vendor/commtiva/z71/proprietary/lib/libOmxAmrEnc.so:system/lib/libOmxAmrEnc.so \
    vendor/commtiva/z71/proprietary/lib/libOmxEvrcEnc.so:system/lib/libOmxEvrcEnc.so \
    vendor/commtiva/z71/proprietary/lib/libOmxQcelp13Enc.so:system/lib/libOmxQcelp13Enc.so \
    vendor/commtiva/z71/proprietary/lib/libOmxWmvDec.so:system/lib/libOmxWmvDec.so

## Hardware properties 
PRODUCT_COPY_FILES += \
    frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/base/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/base/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/base/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/base/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/base/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml

## Camera proprietaries
PRODUCT_COPY_FILES += \
    vendor/commtiva/z71/proprietary/lib/liboemcamera.so:system/lib/liboemcamera.so \
    vendor/commtiva/z71/proprietary/lib/libmmjpeg.so:system/lib/libmmjpeg.so \
    vendor/commtiva/z71/proprietary/lib/libmmipl.so:system/lib/libmmipl.so

## Atheros AR6002 firmware
PRODUCT_COPY_FILES += \
    device/commtiva/z71/prebuilt/data.patch.hw2_0.bin:system/etc/firmware/data.patch.hw2_0.bin \
    device/commtiva/z71/prebuilt/eeprom.bin:system/etc/firmware/eeprom.bin \
    device/commtiva/z71/prebuilt/athwlan.bin.z77:system/etc/firmware/athwlan.bin.z77 \
    device/commtiva/z71/hostapd.conf:system/etc/wifi/hostapd.conf \
    vendor/commtiva/z71/proprietary/bin/hostapd:system/bin/hostapd

## Other libraries and proprietary binaries
PRODUCT_COPY_FILES += \
    vendor/commtiva/z71/proprietary/bin/hci_qcomm_init:system/bin/hci_qcomm_init \
    device/commtiva/z71/prebuilt/init.qcom.bt.sh:system/bin/init.qcom.bt.sh \
    vendor/commtiva/z71/proprietary/lib/libms3c_yamaha.so:system/lib/libms3c_yamaha.so \
    vendor/commtiva/z71/proprietary/lib/libsensor_yamaha.so:system/lib/libsensor_yamaha.so \
    device/commtiva/z71/ms3c_charger_offset.cfg:system/etc/ms3c_charger_offset.cfg \
    device/commtiva/z71/ms3c_transformation.cfg:system/etc/ms3c_transformation.cfg \
    vendor/commtiva/z71/proprietary/bin/updateSensorNV:system/bin/updateSensorNV \
    vendor/commtiva/z71/proprietary/bin/sensorserver_yamaha:system/bin/sensorserver_yamaha \
    device/commtiva/z71/prebuilt/sensordaemon:system/bin/sensordaemon \
    vendor/commtiva/z71/proprietary/lib/hw/sensors.qcom.so:system/lib/hw/sensors.qcom.so \
    vendor/commtiva/z71/proprietary/bin/gsensorcalibration:system/bin/gsensorcalibration \
    device/commtiva/z71/prebuilt/SensorCalibration.apk:system/app/SensorCalibration.apk \
    device/commtiva/z71/AutoVolumeControl.txt:system/etc/AutoVolumeControl.txt \
    device/commtiva/z71/AudioFilter.csv:system/etc/AudioFilter.csv \
    vendor/commtiva/z71/proprietary/lib/liba2dp.so:system/lib/liba2dp.so \
    vendor/commtiva/z71/proprietary/lib/libaudioeq.so:system/lib/libaudioeq.so \
    vendor/commtiva/z71/proprietary/lib/egl/egl.cfg:system/lib/egl/egl.cfg \
    vendor/commtiva/z71/proprietary/lib/egl/libGLESv1_CM_adreno200.so:system/lib/egl/libGLESv1_CM_adreno200.so \
    vendor/commtiva/z71/proprietary/lib/egl/libq3dtools_adreno200.so:system/lib/egl/libq3dtools_adreno200.so \
    vendor/commtiva/z71/proprietary/lib/egl/libEGL_adreno200.so:system/lib/egl/libEGL_adreno200.so \
    vendor/commtiva/z71/proprietary/lib/egl/libGLESv2_adreno200.so:system/lib/egl/libGLESv2_adreno200.so \
    vendor/commtiva/z71/proprietary/etc/firmware/yamato_pfp.fw:system/etc/firmware/yamato_pfp.fw \
    vendor/commtiva/z71/proprietary/etc/firmware/yamato_pm4.fw:system/etc/firmware/yamato_pm4.fw \
    vendor/commtiva/z71/proprietary/lib/libgsl.so:system/lib/libgsl.so

PRODUCT_COPY_FILES += \
    device/commtiva/z71/media_profiles.xml:system/etc/media_profiles.xml \
    device/commtiva/z71/dhcpcd.conf:system/etc/dhcpcd/dhcpcd.conf \
    device/commtiva/z71/vold.fstab:system/etc/vold.fstab \
    device/commtiva/z71/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
    device/commtiva/z71/7x27_kybd.kl:system/usr/keylayout/7x27_kybd.kl


$(call inherit-product, build/target/product/full_base.mk)

PRODUCT_BUILD_PROP_OVERRIDES += BUILD_UTC_DATE=0
PRODUCT_NAME := z71
PRODUCT_DEVICE := z71
PRODUCT_MODEL := Commtiva Z71
