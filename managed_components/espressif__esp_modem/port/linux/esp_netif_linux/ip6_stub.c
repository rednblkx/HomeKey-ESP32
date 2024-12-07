#include "lwip/ip6.h"

err_t
ip6_output_if(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
              u8_t hl, u8_t tc,
              u8_t nexth, struct netif *netif)
{
    return ERR_OK;
}

struct netif *
ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest)
{
    return NULL;
}

err_t
ip6_output_if_src(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
                  u8_t hl, u8_t tc,
                  u8_t nexth, struct netif *netif)
{
    return ERR_OK;
}

const ip_addr_t *
ip6_select_source_address(struct netif *netif, const ip6_addr_t *dest)
{
    return NULL;
}

err_t
ip6_options_add_hbh_ra(struct pbuf *p, u8_t nexth, u8_t value)
{
    return ERR_OK;
}
