/*---------------------------------------------------------------------------*/
/* Link-layer options
 */

/* IEEE802154_CONF_PANID defines the default PAN ID for IEEE 802.15.4 networks */
#ifndef IEEE802154_CONF_PANID
#define IEEE802154_CONF_PANID 0xF00D
#endif /* IEEE802154_CONF_PANID */

/* IEEE802154_CONF_DEFAULT_CHANNEL defines the default channel for IEEE 802.15.4
 * networks, for MAC layers without a channel selection or channel hopping
 * mechanism. Current 802.15.4 MAC layers:
 * - CSMA: uses IEEE802154_CONF_DEFAULT_CHANNEL
 * - TSCH: uses its own TSCH_DEFAULT_HOPPING_SEQUENCE instead
 */
#ifndef IEEE802154_CONF_DEFAULT_CHANNEL
#define IEEE802154_CONF_DEFAULT_CHANNEL 15
#endif /* IEEE802154_CONF_DEF_CHANNEL */
