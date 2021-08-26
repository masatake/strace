#include "defs.h"
#include <linux/vfio.h>

struct vfio_priv_data {
	bool iommu_type1;
};

int
vfio_ioctl_decode_command_number(int code,
				 struct tcb *tcp, int fd, struct fd_priv_data *fd_data)
{
	if (fd_data
	    && fd_data->miscdev_name
	    && strcmp(fd_data->miscdev_name, "vfio") == 0) {
		char *device = get_fdinfo(tcp->pid, fd, "vfio-driver:\t");
		if (!device)
			return 0;
		struct vfio_priv_data *priv = xmalloc(sizeof(struct vfio_priv_data));
		priv->iommu_type1 = false;
		set_tcb_priv_data(tcp, priv, free,
				  vfio_ioctl_decode_command_number);
		if (strcmp(device, "vfio-iommu-type1") == 0) {
			priv->iommu_type1 = true;
			switch (code) {
			case VFIO_IOMMU_MAP_DMA:
				free(device);
				tprints("VFIO_IOMMU_MAP_DMA");
				return IOCTL_NUMBER_STOP_LOOKUP;
			}
		}
		free(device);
	}
	return 0;
}
