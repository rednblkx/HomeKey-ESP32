#include <string.h>
#include <unistd.h>
#include "netif/ppp/pppos.h"
#include "lwip/ip6.h"
#include "lwip/tcpip.h"
#include "netif/ppp/pppos.h"
#include "lwip/dns.h"
#include "esp_netif.h"

#define BUF_SIZE 1518

void ppp_init(void);

static const unsigned char ip6_header[4] = { 0, 0, 0x86, 0xdd };  // Ethernet (IPv6)
static const unsigned char ip4_header[4] = { 0, 0, 0x08, 0 };     // Ethernet (IPv4)

static esp_netif_t *esp_netif = NULL;
static struct netif pppos_netif;
static ppp_pcb *ppp;

static void ppp_link_status_cb(ppp_pcb *pcb, int err_code, void *ctx)
{
    struct netif *pppif = ppp_netif(pcb);
    LWIP_UNUSED_ARG(ctx);

    switch (err_code) {
    case PPPERR_NONE: {             /* No error. */
#if LWIP_DNS
        const ip_addr_t *ns;
#endif /* LWIP_DNS */
        fprintf(stderr, "ppp_link_status_cb: PPPERR_NONE\n\r");
#if LWIP_IPV4
        fprintf(stderr, "   our_ip4addr = %s\n\r", ip4addr_ntoa(netif_ip4_addr(pppif)));
        fprintf(stderr, "   his_ipaddr  = %s\n\r", ip4addr_ntoa(netif_ip4_gw(pppif)));
        fprintf(stderr, "   netmask     = %s\n\r", ip4addr_ntoa(netif_ip4_netmask(pppif)));
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
        fprintf(stderr, "   our_ip6addr = %s\n\r", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* LWIP_IPV6 */

#if LWIP_DNS
        ns = dns_getserver(0);
        fprintf(stderr, "   dns1        = %s\n\r", ipaddr_ntoa(ns));
        ns = dns_getserver(1);
        fprintf(stderr, "   dns2        = %s\n\r", ipaddr_ntoa(ns));
#endif /* LWIP_DNS */
#if PPP_IPV6_SUPPORT
        fprintf(stderr, "   our6_ipaddr = %s\n\r", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* PPP_IPV6_SUPPORT */
    }
    break;

    case PPPERR_PARAM:             /* Invalid parameter. */
        printf("ppp_link_status_cb: PPPERR_PARAM\n");
        break;

    case PPPERR_OPEN:              /* Unable to open PPP session. */
        printf("ppp_link_status_cb: PPPERR_OPEN\n");
        break;

    case PPPERR_DEVICE:            /* Invalid I/O device for PPP. */
        printf("ppp_link_status_cb: PPPERR_DEVICE\n");
        break;

    case PPPERR_ALLOC:             /* Unable to allocate resources. */
        printf("ppp_link_status_cb: PPPERR_ALLOC\n");
        break;

    case PPPERR_USER:              /* User interrupt. */
        printf("ppp_link_status_cb: PPPERR_USER\n");
        break;

    case PPPERR_CONNECT:           /* Connection lost. */
        printf("ppp_link_status_cb: PPPERR_CONNECT\n");
        break;

    case PPPERR_AUTHFAIL:          /* Failed authentication challenge. */
        printf("ppp_link_status_cb: PPPERR_AUTHFAIL\n");
        break;

    case PPPERR_PROTOCOL:          /* Failed to meet protocol. */
        printf("ppp_link_status_cb: PPPERR_PROTOCOL\n");
        break;

    case PPPERR_PEERDEAD:          /* Connection timeout. */
        printf("ppp_link_status_cb: PPPERR_PEERDEAD\n");
        break;

    case PPPERR_IDLETIMEOUT:       /* Idle Timeout. */
        printf("ppp_link_status_cb: PPPERR_IDLETIMEOUT\n");
        break;

    case PPPERR_CONNECTTIME:       /* PPPERR_CONNECTTIME. */
        printf("ppp_link_status_cb: PPPERR_CONNECTTIME\n");
        break;

    case PPPERR_LOOPBACK:          /* Connection timeout. */
        printf("ppp_link_status_cb: PPPERR_LOOPBACK\n");
        break;

    default:
        printf("ppp_link_status_cb: unknown errCode %d\n", err_code);
        break;
    }
}

static u32_t ppp_output_cb(struct ppp_pcb_s *pcb, const void *data, u32_t len, void *ctx)
{
    esp_netif_t *netif = (esp_netif_t *)ctx;
    if (netif->transmit) {
        return netif->transmit(netif->ctx, (uint8_t *)data, len);
    }
    return  0;
}

int esp_netif_receive(esp_netif_t *netif, uint8_t *data, size_t len)
{
    pppos_input(ppp, data, len);
    return 1;
}

int ppp_netif_init(esp_netif_t *netif)
{
    // Init necessary units of lwip (no need for the tcpip thread)
    sys_init();
    mem_init();
    memp_init();
    netif_init();
    dns_init();
    ppp_init();
    sys_timeouts_init();

    // init and start connection attempts on PPP interface
    ppp = pppos_create(&pppos_netif, ppp_output_cb, ppp_link_status_cb, (void *)netif);
    if (ppp == NULL) {
        return 0;
    }
    ppp_connect(ppp, 0);
    ppp_set_usepeerdns(ppp, 1);
    esp_netif = netif;
    return 1;
}

static err_t tun_input(struct pbuf *p, const unsigned char tun_header[4])
{
    struct pbuf *n;
    size_t offset = 4;
    memcpy(esp_netif->in_buf, tun_header, offset);
    for (n = p; n; n = n->next) {
        memcpy(esp_netif->in_buf + offset, n->payload, n->len);
        offset += n->len;
    }
    if (write(esp_netif->fd, esp_netif->in_buf, offset) != offset) {
        pbuf_free(p);
        return ERR_ABRT;
    }
    pbuf_free(p);
    return ERR_OK;
}

err_t ip6_input(struct pbuf *p, struct netif *inp)
{
    return tun_input(p, ip6_header);
}

err_t ip4_input(struct pbuf *p, struct netif *inp)
{
    return tun_input(p, ip4_header);
}

int tun_read(void)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(esp_netif->fd, &fds);
    struct timeval tv = { .tv_usec = 0, .tv_sec = 1 };

    if (select(esp_netif->fd + 1, &fds, NULL, NULL, &tv) <= 0) {
        sys_check_timeouts();
        return 0;
    }

    struct pbuf *p;
    ssize_t len = read(esp_netif->fd, esp_netif->out_buf, BUF_SIZE);
    if (len < 0) {
        perror("read returned -1");
        return -1;
    }

    if (len <= 4) {
        return -1;
    }
    len -= 4;
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if (p == NULL) {
        perror("Cannot allocate pbuf");
        return -1;
    }
    pbuf_take(p, esp_netif->out_buf + 4, len);
    if (memcmp(esp_netif->out_buf, ip6_header, 4) == 0) {
        pppos_netif.output_ip6(&pppos_netif, p, NULL);
    } else if (memcmp(esp_netif->out_buf, ip4_header, 4) == 0) {
        pppos_netif.output(&pppos_netif, p, NULL);
    } else {
        printf("Unknown protocol %x %x\n", esp_netif->out_buf[2], esp_netif->out_buf[3]);
    }
    pbuf_free(p);
    return 1;
}
