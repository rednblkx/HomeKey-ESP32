#include "lwip/ip4.h"

err_t
ip4_output_if(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
              u8_t ttl, u8_t tos,
              u8_t proto, struct netif *netif)
{
    return ERR_OK;
}

struct netif *
ip4_route(const ip4_addr_t *dest)
{
    return NULL;
}

err_t
ip4_output_if_src(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
                  u8_t ttl, u8_t tos,
                  u8_t proto, struct netif *netif)
{
    return ERR_OK;
}
