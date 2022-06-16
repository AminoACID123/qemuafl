/* virtual Bluetooth Controller */

#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
// #include <glib.h>

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "qemu/cutils.h"
#include "qemu/error-report.h"
#include "hw/usb.h"
#include "hw/usb/desc.h"

#include "afl.h"


static const USBDescIface desc_iface_bluetooth = {
    .bInterfaceNumber              = 0,
    .bNumEndpoints                 = 3,
    .bInterfaceClass               = 0xe0, /* Wireless Controller */
    .bInterfaceSubClass            = 0x01, /* RF Controller */
    .bInterfaceProtocol            = 0x01, /* Bluetooth Primary Controller */
    .eps = (USBDescEndpoint[]) {
        /* HCI Events */
        {
            .bEndpointAddress      = 0x81,
            .bmAttributes          = USB_ENDPOINT_XFER_INT,
            .wMaxPacketSize        = 16,
            .bInterval             = 0x0a,
        },
        /* ACL Data (Bulk in) */
        {
            .bEndpointAddress      = 0x82,
            .bmAttributes          = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize        = 64,
            .bInterval             = 0x0a,
        },
        /* ACL Data (Bulk Out) */
        {
            .bEndpointAddress      = 0x02,
            .bmAttributes          = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize        = 64,
            .bInterval             = 0x0a,
        },
    },
};

static const USBDescDevice desc_device_bluetooth = {

    .bcdUSB                          = 0x0200,
    .bDeviceClass                    = 0xe0,
    .bDeviceSubClass                 = 0x01,
    .bDeviceProtocol                 = 0x01,
    .bMaxPacketSize0                 = 64,
    .bNumConfigurations              = 1,
    .confs = (USBDescConfig[]) {
        {
            .bNumInterfaces        = 1,
            .bConfigurationValue   = 1,
            .bmAttributes          = 0,
            .bMaxPower             = 100,
            .nif = 1,
            .ifs = &desc_iface_bluetooth,
        },
    },
};

static const USBDesc desc_bluetooth = {
    .full = &desc_device_bluetooth,
};

static void usb_bt_realize(USBDevice *dev, Error **errp)
{
    // dev->speedmask |= USB_SPEED_MASK_LOW;
    dev->speedmask |= USB_SPEED_MASK_FULL;
    dev->usb_desc = &desc_bluetooth;
    usb_desc_init(dev);
    // dev->speedmask |= USB_SPEED_MASK_HIGH;

    // reset_afl_usb_fuzz_data();
}

static void usb_bt_handle_control(USBDevice *dev, USBPacket *p,
                                    int request, int value, int index,
                                    int length, uint8_t *data) {

    printf("request: %d\nvalue: %d\nindex: %d\n", request, value, index);
    int ret = usb_desc_handle_control(dev, p, request, value, index, length, data);
    if (ret >= 0) {
        return;
    }
//    PFATAL("Error\n");
//    exit(-1);
}

static void usb_bt_handle_data(USBDevice *dev, USBPacket *p) {

}

static void usb_bt_handle_reset(USBDevice *dev) {

}

static void usb_bt_class_initfn(ObjectClass *klass, void *data)
{
    // DeviceClass *dc = DEVICE_CLASS(klass);
    USBDeviceClass *uc = USB_DEVICE_CLASS(klass);

    // gen_random_bytes(sizeof(USBDescDevice), &desc_fuzz_device);

    uc->product_desc   = "QEMU Virtual Bluetooth Controller";
    // uc->usb_desc = &usbdesc;
    uc->realize = usb_bt_realize;
    uc->handle_reset = usb_bt_handle_reset;
    uc->handle_control = usb_bt_handle_control;
    uc->handle_data = usb_bt_handle_data;

  // set_bit(DEVICE_CATEGORY_INPUT, dc->categories);
}

static const TypeInfo usbbt_info = {
    .name          = "usb-bt",
    .parent        = TYPE_USB_DEVICE,
    .class_init    = usb_bt_class_initfn,
};

static void usb_bt_register_type(void)
{
    type_register_static(&usbbt_info);
}

type_init(usb_bt_register_type)

