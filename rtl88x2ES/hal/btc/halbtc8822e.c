/******************************************************************************
 *
 * Copyright(c) 2016 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#include "mp_precomp.h"

#if (BT_SUPPORT == 1 && COEX_SUPPORT == 1)

static u8 *trace_buf = &gl_btc_trace_buf[0];

/* rssi express in percentage % (dbm = % - 100)  */
static const u8 wl_rssi_step_8822e[] = {60, 50, 44, 30};
static const u8 bt_rssi_step_8822e[] = {8, 15, 20, 25};

/* Shared-Antenna Coex Table */
static const struct btc_coex_table_para table_sant_8822e[] = {
				{0xffffffff, 0xffffffff}, /*case-0*/
				{0x55555555, 0x55555555},
				{0x66555555, 0x66555555},
				{0xaaaaaaaa, 0xaaaaaaaa},
				{0x5a5a5a5a, 0x5a5a5a5a},
				{0xfafafafa, 0xfafafafa}, /*case-5*/
				{0x6a5a5555, 0xaaaaaaaa},
				{0x6a5a56aa, 0x6a5a56aa},
				{0x6a5a5a5a, 0x6a5a5a5a},
				{0x66555555, 0x5a5a5a5a},
				{0x66555555, 0x6a5a5a5a}, /*case-10*/
				{0x66555555, 0xaaaaaaaa},
				{0x66555555, 0x5a5a5aaa},
				{0x66555555, 0x6aaa5aaa},
				{0x66555555, 0xaaaa5aaa},
				{0x66555555, 0xaaaaaaaa}, /*case-15*/
				{0xffff55ff, 0xfafafafa},
				{0xffff55ff, 0x6afa5afa},
				{0xaaffffaa, 0xfafafafa},
				{0xaa5555aa, 0x5a5a5a5a},
				{0xaa5555aa, 0x6a5a5a5a}, /*case-20*/
				{0xaa5555aa, 0xaaaaaaaa},
				{0xffffffff, 0x5a5a5a5a},
				{0xffffffff, 0x5a5a5a5a},
				{0xffffffff, 0x55555555},
				{0xffffffff, 0x5a5a5aaa}, /*case-25*/
				{0x55555555, 0x5a5a5a5a},
				{0x55555555, 0xaaaaaaaa},
				{0x55555555, 0x6a5a6a5a},
				{0x66556655, 0x66556655},
				{0x66556aaa, 0x6a5a6aaa}, /*case-30*/
				{0xffffffff, 0x5aaa5aaa},
				{0x56555555, 0x5a5a5aaa},
				{0xdaffdaff, 0xdaffdaff},
				{0x6a555a5a, 0x5a5a5a5a},
				{0xe5555555, 0xe5555555}, /*case-35*/
				{0xea5a5a5a, 0xea5a5a5a},
				{0xea6a6a6a, 0xea6a6a6a} };

/* Non-Shared-Antenna Coex Table */
static const struct btc_coex_table_para table_nsant_8822e[] = {
				{0xffffffff, 0xffffffff}, /*case-100*/
				{0x55555555, 0x55555555},
				{0x66555555, 0x66555555},
				{0xaaaaaaaa, 0xaaaaaaaa},
				{0x5a5a5a5a, 0x5a5a5a5a},
				{0xfafafafa, 0xfafafafa}, /*case-105*/
				{0x5afa5afa, 0x5afa5afa},
				{0x55555555, 0xfafafafa},
				{0x66555555, 0xfafafafa},
				{0x66555555, 0x5a5a5a5a},
				{0x66555555, 0x6a5a5a5a}, /*case-110*/
				{0x66555555, 0xaaaaaaaa},
				{0xffff55ff, 0xfafafafa},
				{0xffff55ff, 0x5afa5afa},
				{0xffff55ff, 0xaaaaaaaa},
				{0xffff55ff, 0xffff55ff}, /*case-115*/
				{0xaaffffaa, 0x5afa5afa},
				{0xaaffffaa, 0xaaaaaaaa},
				{0xffffffff, 0xfafafafa},
				{0xffffffff, 0x5afa5afa},
				{0xffffffff, 0xaaaaaaaa},/*case-120*/
				{0x55ff55ff, 0x5afa5afa},
				{0x55ff55ff, 0xaaaaaaaa},
				{0x55ff55ff, 0x55ff55ff},
				{0x6a555a5a, 0xfafafafa} };

/* Shared-Antenna TDMA*/
static const struct btc_tdma_para tdma_sant_8822e[] = {
				{ {0x00, 0x00, 0x00, 0x00, 0x00} }, /*case-0*/
				{ {0x61, 0x45, 0x03, 0x11, 0x11} }, /*case-1*/
				{ {0x61, 0x3a, 0x03, 0x11, 0x11} },
				{ {0x61, 0x30, 0x03, 0x11, 0x11} },
				{ {0x61, 0x20, 0x03, 0x11, 0x11} },
				{ {0x61, 0x10, 0x03, 0x11, 0x11} }, /*case-5*/
				{ {0x61, 0x45, 0x03, 0x11, 0x10} },
				{ {0x61, 0x3a, 0x03, 0x11, 0x10} },
				{ {0x61, 0x30, 0x03, 0x11, 0x10} },
				{ {0x61, 0x20, 0x03, 0x11, 0x10} },
				{ {0x61, 0x10, 0x03, 0x11, 0x10} }, /*case-10*/
				{ {0x61, 0x08, 0x03, 0x11, 0x14} },
				{ {0x61, 0x08, 0x03, 0x10, 0x14} },
				{ {0x51, 0x08, 0x03, 0x10, 0x54} },
				{ {0x51, 0x08, 0x03, 0x10, 0x55} },
				{ {0x51, 0x08, 0x07, 0x10, 0x54} }, /*case-15*/
				{ {0x51, 0x45, 0x03, 0x10, 0x50} },
				{ {0x51, 0x3a, 0x03, 0x10, 0x50} },
				{ {0x51, 0x30, 0x03, 0x10, 0x50} },
				{ {0x51, 0x20, 0x03, 0x10, 0x50} },
				{ {0x51, 0x10, 0x03, 0x10, 0x50} }, /*case-20*/
				{ {0x51, 0x4a, 0x03, 0x10, 0x50} },
				{ {0x51, 0x0c, 0x03, 0x10, 0x54} },
				{ {0x55, 0x08, 0x03, 0x10, 0x54} },
				{ {0x65, 0x10, 0x03, 0x11, 0x10} },
				{ {0x51, 0x10, 0x03, 0x10, 0x51} }, /*case-25*/
				{ {0x51, 0x08, 0x03, 0x10, 0x50} },
				{ {0x61, 0x08, 0x03, 0x11, 0x11} } };


/* Non-Shared-Antenna TDMA*/
static const struct btc_tdma_para tdma_nsant_8822e[] = {
				{ {0x00, 0x00, 0x00, 0x00, 0x00} }, /*case-100*/
				{ {0x61, 0x45, 0x03, 0x11, 0x11} }, /*case-101*/
				{ {0x61, 0x3a, 0x03, 0x11, 0x11} },
				{ {0x61, 0x30, 0x03, 0x11, 0x11} },
				{ {0x61, 0x20, 0x03, 0x11, 0x11} },
				{ {0x61, 0x10, 0x03, 0x11, 0x11} }, /*case-105*/
				{ {0x61, 0x45, 0x03, 0x11, 0x10} },
				{ {0x61, 0x3a, 0x03, 0x11, 0x10} },
				{ {0x61, 0x30, 0x03, 0x11, 0x10} },
				{ {0x61, 0x20, 0x03, 0x11, 0x10} },
				{ {0x61, 0x10, 0x03, 0x11, 0x10} }, /*case-110*/
				{ {0x61, 0x08, 0x03, 0x11, 0x14} },
				{ {0x61, 0x08, 0x03, 0x10, 0x14} },
				{ {0x51, 0x08, 0x03, 0x10, 0x54} },
				{ {0x51, 0x08, 0x03, 0x10, 0x55} },
				{ {0x51, 0x08, 0x07, 0x10, 0x54} }, /*case-115*/
				{ {0x51, 0x45, 0x03, 0x10, 0x50} },
				{ {0x51, 0x3a, 0x03, 0x10, 0x50} },
				{ {0x51, 0x30, 0x03, 0x10, 0x50} },
				{ {0x51, 0x20, 0x03, 0x10, 0x50} },
				{ {0x51, 0x10, 0x03, 0x10, 0x50} }, /*case-120*/
				{ {0x51, 0x08, 0x03, 0x10, 0x50} },
				{ {0x61, 0x30, 0x03, 0x10, 0x11} },
				{ {0x61, 0x08, 0x03, 0x10, 0x11} },
				{ {0x61, 0x08, 0x07, 0x10, 0x14} },
				{ {0x61, 0x08, 0x03, 0x10, 0x10} }, /*case-125*/
				{ {0x61, 0x08, 0x03, 0x11, 0x15} } };

/* wl_tx_dec_power, bt_tx_dec_power, wl_rx_gain, bt_rx_lna_constrain */
static const struct btc_rf_para rf_para_tx_8822e[] = {
				{0, 0, FALSE, 7},  /* for normal */
				{0, 16, FALSE, 7}, /* for WL-CPT */
				{16, 4, TRUE, 4},  /* 2 for RCU SDR */
				{15, 5, TRUE, 4},
				{7, 8, TRUE, 4},
				{6, 10, TRUE, 4},
				{16, 4, TRUE, 4}, /* 6 for RCU OFC */
				{15, 5, TRUE, 4},
				{7, 8, TRUE, 4},
				{6, 10, TRUE, 4},
				{16, 4, TRUE, 4}, /* 10 for A2DP SDR */
				{15, 5, TRUE, 4},
				{7, 8, TRUE, 4},
				{6, 10, TRUE, 4},
				{16, 4, TRUE, 4}, /* 14 for A2DP OFC */
				{15, 5, TRUE, 4},
				{7, 8, TRUE, 4},
				{6, 10, TRUE, 4},
				{16, 4, TRUE, 4}, /* 18 for A2DP+RCU SDR */
				{15, 5, TRUE, 4},
				{7, 8, TRUE, 4},
				{6, 10, TRUE, 4},
				{16, 4, TRUE, 4}, /* 22 for A2DP+RCU OFC */
				{15, 5, TRUE, 4},
				{7, 8, TRUE, 4},
				{6, 10, TRUE, 4} };

static const struct btc_rf_para rf_para_rx_8822e[] = {
				{0, 0, FALSE, 7},  /* for normal */
				{0, 16, FALSE, 7}, /* for WL-CPT */
				{14, 5, TRUE, 5}, /* 2 for RCU SDR */
				{13, 6, TRUE, 5},
				{6, 9, TRUE, 5},
				{4, 11, TRUE, 5},
				{16, 4, TRUE, 5}, /* 6 for RCU OFC */
				{15, 5, TRUE, 5},
				{7, 8, TRUE, 5},
				{6, 10, TRUE, 5},
				{16, 4, TRUE, 5}, /* 10 for A2DP SDR */
				{15, 5, TRUE, 5},
				{7, 8, TRUE, 5},
				{6, 10, TRUE, 5},
				{16, 4, TRUE, 5}, /* 14 for A2DP OFC */
				{15, 5, TRUE, 5},
				{7, 8, TRUE, 5},
				{6, 10, TRUE, 5},
				{16, 4, TRUE, 5}, /* 18 for A2DP+RCU SDR */
				{15, 5, TRUE, 5},
				{7, 8, TRUE, 5},
				{6, 10, TRUE, 5},
				{16, 4, TRUE, 5}, /* 22 for A2DP+RCU OFC */
				{15, 5, TRUE, 5},
				{7, 8, TRUE, 5},
				{6, 10, TRUE, 5} };

const struct btc_5g_afh_map afh_5g_8822e[] = { {0, 0, 0} };

const struct btc_chip_para btc_chip_para_8822e = {
	"8822e",				/*.chip_name */
	20220722,				/*.para_ver_date */
	0x04,					/*.para_ver */
	0x03,					/* bt_desired_ver */
	0x10002,				/* wl_desired_ver */
	TRUE,					/* scbd_support */
	0xaa,					/* scbd_reg*/
	BTC_SCBD_16_BIT,			/* scbd_bit_num */
	TRUE,					/* mailbox_support*/
	TRUE,					/* lte_indirect_access */
	TRUE,					/* new_scbd10_def */
	BTC_INDIRECT_1700,			/* indirect_type */
	BTC_PSTDMA_FORCE_LPSOFF,		/* pstdma_type */
	BTC_BTRSSI_DBM,				/* bt_rssi_type */
	15,					/*.ant_isolation */
	2,					/*.rssi_tolerance */
	2,					/* rx_path_num */
	ARRAY_SIZE(wl_rssi_step_8822e),		/*.wl_rssi_step_num */
	wl_rssi_step_8822e,			/*.wl_rssi_step */
	ARRAY_SIZE(bt_rssi_step_8822e),		/*.bt_rssi_step_num */
	bt_rssi_step_8822e,			/*.bt_rssi_step */
	ARRAY_SIZE(table_sant_8822e),		/*.table_sant_num */
	table_sant_8822e,			/*.table_sant = */
	ARRAY_SIZE(table_nsant_8822e),		/*.table_nsant_num */
	table_nsant_8822e,			/*.table_nsant = */
	ARRAY_SIZE(tdma_sant_8822e),		/*.tdma_sant_num */
	tdma_sant_8822e,			/*.tdma_sant = */
	ARRAY_SIZE(tdma_nsant_8822e),		/*.tdma_nsant_num */
	tdma_nsant_8822e,			/*.tdma_nsant */
	ARRAY_SIZE(rf_para_tx_8822e),		/* wl_rf_para_tx_num */
	rf_para_tx_8822e,		        /* wl_rf_para_tx */
	rf_para_rx_8822e,		        /* wl_rf_para_rx */
	0x24,					/*.bt_afh_span_bw20 */
	0x36,					/*.bt_afh_span_bw40 */
	ARRAY_SIZE(afh_5g_8822e),		/*.afh_5g_num */
	afh_5g_8822e,				/*.afh_5g */
	halbtc8822e_chip_setup			/* chip_setup function */
};

const u32 btc_8822e_bt_idle_radioa[] = {
	0xEF, 0x10000,				/* WRITE_EN=1 */

	/* POW_TX_PA=0, POW_LNA2_BTS1=0 */
	0x33, 0x0000F,				/* LNA=111 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x00700,
	0x33, 0x0000E,				/* LNA=111 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x00700,
	0x33, 0x0000D,				/* LNA=110 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x00700,
	0x33, 0x0000C,				/* LNA=110 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x00700,
	0x33, 0x0000B,				/* LNA=101 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x92500,
	0x33, 0x0000A,				/* LNA=101 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x92500,
	0x33, 0x00009,				/* LNA=100 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x93400,
	0x33, 0x00008,				/* LNA=100 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x93400,
	0x33, 0x00007,				/* LNA=011 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xA8300,
	0x33, 0x00006,				/* LNA=011 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0xA8300,
	0x33, 0x00005,				/* LNA=010 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xBB300,
	0x33, 0x00004,				/* LNA=010 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0xBB300,
	0x33, 0x00003,				/* LNA=001 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x93200,
	0x33, 0x00002,				/* LNA=001 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x93200,
	0x33, 0x00001,				/* LNA=000 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xC3100,
	0x33, 0x00000,				/* LNA=000 + TIA1 */
	0x3E, 0x0000C,
	0x3F, 0xC3100,

	/* POW_TX_PA=1, POW_LNA2_BTS1=0 */
	0x33, 0x0001F,				/* LNA=111 + TIA1*/
	0x3E, 0x0000F,
	0x3F, 0x00700,
	0x33, 0x0001E,				/* LNA=111 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x00700,
	0x33, 0x0001D,				/* LNA=110 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x00700,
	0x33, 0x0001C,				/* LNA=110 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x00700,
	0x33, 0x0001B,				/* LNA=101 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x92500,
	0x33, 0x0001A,				/* LNA=101 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x92500,
	0x33, 0x00019,				/* LNA=100 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x93400,
	0x33, 0x00018,				/* LNA=100 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x93400,
	0x33, 0x00017,				/* LNA=011 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xA8300,
	0x33, 0x00016,				/* LNA=011 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0xA8300,
	0x33, 0x00015,				/* LNA=010 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xBB300,
	0x33, 0x00014,				/* LNA=010 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0xBB300,
	0x33, 0x00013,				/* LNA=001 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x93200,
	0x33, 0x00012,				/* LNA=001 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x93200,
	0x33, 0x00011,				/* LNA=000 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xC3100,
	0x33, 0x00010,				/* LNA=000 + TIA1 */
	0x3E, 0x0000C,
	0x3F, 0xC3100,

	0xEF, 0x00000,				/* WRITE_EN=0 */
};

const u32 btc_8822e_bt_idle_radiob[] = {
	0xEF, 0x10000,				/* WRITE_EN=1 */

	/* POW_TX_PA=0, POW_LNA2_BTS1=0 */
	0x33, 0x0000F,				/* LNA=111 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x00700,
	0x33, 0x0000E,				/* LNA=111 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x00700,
	0x33, 0x0000D,				/* LNA=110 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x00700,
	0x33, 0x0000C,				/* LNA=110 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x00700,
	0x33, 0x0000B,				/* LNA=101 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x92500,
	0x33, 0x0000A,				/* LNA=101 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x92500,
	0x33, 0x00009,				/* LNA=100 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x93400,
	0x33, 0x00008,				/* LNA=100 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x93400,
	0x33, 0x00007,				/* LNA=011 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xA8300,
	0x33, 0x00006,				/* LNA=011 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0xA8300,
	0x33, 0x00005,				/* LNA=010 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xBB300,
	0x33, 0x00004,				/* LNA=010 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0xBB300,
	0x33, 0x00003,				/* LNA=001 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x93200,
	0x33, 0x00002,				/* LNA=001 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x93200,
	0x33, 0x00001,				/* LNA=000 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xC3100,
	0x33, 0x00000,				/* LNA=000 + TIA1 */
	0x3E, 0x0000C,
	0x3F, 0xC3100,

	/* POW_TX_PA=1, POW_LNA2_BTS1=0 */
	0x33, 0x0001F,				/* LNA=111 + TIA1*/
	0x3E, 0x0000F,
	0x3F, 0x00700,
	0x33, 0x0001E,				/* LNA=111 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x00700,
	0x33, 0x0001D,				/* LNA=110 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x00700,
	0x33, 0x0001C,				/* LNA=110 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x00700,
	0x33, 0x0001B,				/* LNA=101 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x92500,
	0x33, 0x0001A,				/* LNA=101 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x92500,
	0x33, 0x00019,				/* LNA=100 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x93400,
	0x33, 0x00018,				/* LNA=100 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x93400,
	0x33, 0x00017,				/* LNA=011 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xA8300,
	0x33, 0x00016,				/* LNA=011 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0xA8300,
	0x33, 0x00015,				/* LNA=010 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xBB300,
	0x33, 0x00014,				/* LNA=010 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0xBB300,
	0x33, 0x00013,				/* LNA=001 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x93200,
	0x33, 0x00012,				/* LNA=001 + TIA0 */
	0x3E, 0x0000C,
	0x3F, 0x93200,
	0x33, 0x00011,				/* LNA=000 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0xC3100,
	0x33, 0x00010,				/* LNA=000 + TIA1 */
	0x3E, 0x0000C,
	0x3F, 0xC3100,

	/* POW_TX_PA=0, POW_LNA2_BTS1=1 */
	0x33, 0x0002F,				/* LNA=111 + TIA1 */
	0x3E, 0x0000F,
	0x3F, 0x00700,
	0x33, 0x0002E,				/* LNA=111 + TIA0 - LNA2 */
	0x3E, 0x0000C,
	0x3F, 0x00700,
	0x33, 0x0002D,				/* LNA=110 + TIA1 - LNA2 */
	0x3E, 0x0000F,
	0x3F, 0x00700,
	0x33, 0x0002C,				/* LNA=110 + TIA0 - LNA2 */
	0x3E, 0x0000C,
	0x3F, 0x00700,
	0x33, 0x0002B,				/* LNA=101 + TIA1 - LNA2 */
	0x3E, 0x0000F,
	0x3F, 0x92500,
	0x33, 0x0002A,				/* LNA=101 + TIA0 - LNA2 */
	0x3E, 0x0000C,
	0x3F, 0x92500,
	0x33, 0x00029,				/* LNA=100 + TIA1 - LNA2 */
	0x3E, 0x0000F,
	0x3F, 0x93400,
	0x33, 0x00028,				/* LNA=100 + TIA0 - LNA2 */
	0x3E, 0x0000C,
	0x3F, 0x93400,
	0x33, 0x00027,				/* LNA=011 + TIA1 - LNA2 */
	0x3E, 0x0000F,
	0x3F, 0xA8300,
	0x33, 0x00026,				/* LNA=011 + TIA0 - LNA2 */
	0x3E, 0x0000C,
	0x3F, 0xA8300,
	0x33, 0x00025,				/* LNA=010 + TIA1 - LNA2 */
	0x3E, 0x0000F,
	0x3F, 0xBB300,
	0x33, 0x00024,				/* LNA=010 + TIA0 - LNA2 */
	0x3E, 0x0000C,
	0x3F, 0xBB300,
	0x33, 0x00023,				/* LNA=001 + TIA1 - LNA2 */
	0x3E, 0x0000F,
	0x3F, 0x93200,
	0x33, 0x00022,				/* LNA=001 + TIA0 - LNA2 */
	0x3E, 0x0000C,
	0x3F, 0x93200,
	0x33, 0x00021,				/* LNA=000 + TIA1 - LNA2 */
	0x3E, 0x0000F,
	0x3F, 0xC3100,
	0x33, 0x00020,				/* LNA=000 + TIA0 - LNA2 */
	0x3E, 0x0000C,
	0x3F, 0xC3100,

	0xEF, 0x00000,				/* WRITE_EN=0 */
};

const u32 btc_8822e_bt_connect_radioa[] = {
	0xEF, 0x10000,				/* WRITE_EN=1 */

	/* POW_TX_PA=0, POW_LNA2_BTS1=0 */
	0x33, 0x0000F,				/* LNA=111 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x00700,
	0x33, 0x0000E,				/* LNA=111 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x00700,
	0x33, 0x0000D,				/* LNA=110 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x00700,
	0x33, 0x0000C,				/* LNA=110 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x00700,
	0x33, 0x0000B,				/* LNA=101 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x92500,
	0x33, 0x0000A,				/* LNA=101 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x92500,
	0x33, 0x00009,				/* LNA=100 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x93400,
	0x33, 0x00008,				/* LNA=100 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x93400,
	0x33, 0x00007,				/* LNA=011 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xA8300,
	0x33, 0x00006,				/* LNA=011 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xA8300,
	0x33, 0x00005,				/* LNA=010 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xBB300,
	0x33, 0x00004,				/* LNA=010 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xBB300,
	0x33, 0x00003,				/* LNA=001 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x93200,
	0x33, 0x00002,				/* LNA=001 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x93200,
	0x33, 0x00001,				/* LNA=000 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xC3100,
	0x33, 0x00000,				/* LNA=000 + TIA1 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xC3100,

	/* POW_TX_PA=1, POW_LNA2_BTS1=0 */
	0x33, 0x0001F,				/* LNA=111 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x00700,
	0x33, 0x0001E,				/* LNA=111 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x00700,
	0x33, 0x0001D,				/* LNA=110 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x00700,
	0x33, 0x0001C,				/* LNA=110 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x00700,
	0x33, 0x0001B,				/* LNA=101 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x92500,
	0x33, 0x0001A,				/* LNA=101 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x92500,
	0x33, 0x00019,				/* LNA=100 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x93400,
	0x33, 0x00018,				/* LNA=100 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x93400,
	0x33, 0x00017,				/* LNA=011 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xA8300,
	0x33, 0x00016,				/* LNA=011 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xA8300,
	0x33, 0x00015,				/* LNA=010 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xBB300,
	0x33, 0x00014,				/* LNA=010 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xBB300,
	0x33, 0x00013,				/* LNA=001 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x93200,
	0x33, 0x00012,				/* LNA=001 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x93200,
	0x33, 0x00011,				/* LNA=000 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xC3100,
	0x33, 0x00010,				/* LNA=000 + TIA1 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xC3100,

	0xEF, 0x00000,				/* WRITE_EN=0 */
};

const u32 btc_8822e_bt_connect_radiob[] = {
	0xEF, 0x10000,				/* WRITE_EN=1 */

	/* POW_TX_PA=0, POW_LNA2_BTS1=0 */
	0x33, 0x0000F,				/* LNA=111 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x00700,
	0x33, 0x0000E,				/* LNA=111 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x00700,
	0x33, 0x0000D,				/* LNA=110 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x00700,
	0x33, 0x0000C,				/* LNA=110 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x00700,
	0x33, 0x0000B,				/* LNA=101 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x92500,
	0x33, 0x0000A,				/* LNA=101 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x92500,
	0x33, 0x00009,				/* LNA=100 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x93400,
	0x33, 0x00008,				/* LNA=100 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x93400,
	0x33, 0x00007,				/* LNA=011 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xA8300,
	0x33, 0x00006,				/* LNA=011 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xA8300,
	0x33, 0x00005,				/* LNA=010 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xBB300,
	0x33, 0x00004,				/* LNA=010 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xBB300,
	0x33, 0x00003,				/* LNA=001 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x93200,
	0x33, 0x00002,				/* LNA=001 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x93200,
	0x33, 0x00001,				/* LNA=000 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xC3100,
	0x33, 0x00000,				/* LNA=000 + TIA1 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xC3100,

	/* POW_TX_PA=1, POW_LNA2_BTS1=0 */
	0x33, 0x0001F,				/* LNA=111 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x00700,
	0x33, 0x0001E,				/* LNA=111 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x00700,
	0x33, 0x0001D,				/* LNA=110 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x00700,
	0x33, 0x0001C,				/* LNA=110 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x00700,
	0x33, 0x0001B,				/* LNA=101 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x92500,
	0x33, 0x0001A,				/* LNA=101 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x92500,
	0x33, 0x00019,				/* LNA=100 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x93400,
	0x33, 0x00018,				/* LNA=100 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x93400,
	0x33, 0x00017,				/* LNA=011 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xA8300,
	0x33, 0x00016,				/* LNA=011 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xA8300,
	0x33, 0x00015,				/* LNA=010 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xBB300,
	0x33, 0x00014,				/* LNA=010 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xBB300,
	0x33, 0x00013,				/* LNA=001 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x93200,
	0x33, 0x00012,				/* LNA=001 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x93200,
	0x33, 0x00011,				/* LNA=000 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xC3100,
	0x33, 0x00010,				/* LNA=000 + TIA1 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xC3100,

	/* POW_TX_PA=0, POW_LNA2_BTS1=1 */
	0x33, 0x0002F,				/* LNA=111 + TIA1 */
	0x3E, 0x00007,
	0x3F, 0x00700,
	0x33, 0x0002E,				/* LNA=111 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x00700,
	0x33, 0x0002D,				/* LNA=110 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x00700,
	0x33, 0x0002C,				/* LNA=110 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x00700,
	0x33, 0x0002B,				/* LNA=101 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x92500,
	0x33, 0x0002A,				/* LNA=101 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x92500,
	0x33, 0x00029,				/* LNA=100 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x93400,
	0x33, 0x00028,				/* LNA=100 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x93400,
	0x33, 0x00027,				/* LNA=011 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xA8300,
	0x33, 0x00026,				/* LNA=011 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xA8300,
	0x33, 0x00025,				/* LNA=010 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xBB300,
	0x33, 0x00024,				/* LNA=010 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xBB300,
	0x33, 0x00023,				/* LNA=001 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0x93200,
	0x33, 0x00022,				/* LNA=001 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0x93200,
	0x33, 0x00021,				/* LNA=000 + TIA1 - LNA2 */
	0x3E, 0x00007,
	0x3F, 0xC3100,
	0x33, 0x00020,				/* LNA=000 + TIA0 - LNA2 */
	0x3E, 0x00004,
	0x3F, 0xC3100,

	0xEF, 0x00000,				/* WRITE_EN=0 */
};

void halbtc8822e_cfg_init(struct btc_coexist *btc)
{
	u8 u8tmp = 0;

	/* enable TBTT nterrupt */
	btc->btc_write_1byte_bitmask(btc, 0x550, 0x8, 0x1);

	/* BT report packet sample rate	 */
	/* 0x790[5:0]=0x5 */
	btc->btc_write_1byte(btc, 0x790, 0x5);

	/* Enable BT counter statistics */
	btc->btc_write_1byte(btc, 0x778, 0x1);

	/* Enable PTA (3-wire function form BT side) */
	btc->btc_write_1byte_bitmask(btc, 0x40, 0x20, 0x1);
	btc->btc_write_1byte_bitmask(btc, 0x41, 0x02, 0x1);

	/* Enable PTA (tx/rx signal form WiFi side) */
	btc->btc_write_1byte_bitmask(btc, 0x4c6, BIT(4), 0x1);
	btc->btc_write_1byte_bitmask(btc, 0x4c6, BIT(5), 0x0);
	/*GNT_BT=1 while select both */
	btc->btc_write_1byte_bitmask(btc, 0x763, BIT(4), 0x1);

	/* BT_CCA = ~GNT_WL_BB, (not or GNT_BT_BB, LTE_Rx */
	btc->btc_write_1byte_bitmask(btc, 0x4fc, 0x3, 0x0);

	/* To avoid RF parameter error */
	btc->btc_set_rf_reg(btc, BTC_RF_B, 0x1, 0xfffff, 0x40000);
}

void halbtc8822e_cfg_ant_switch(struct btc_coexist *btc)
{}

void halbtc8822e_cfg_gnt_fix(struct btc_coexist *btc)
{
	struct btc_coex_sta *coex_sta = &btc->coex_sta;
	struct btc_wifi_link_info_ext *link_info_ext = &btc->wifi_link_info_ext;
	u32 val = 0x40000;

	/* Because WL-S1 5G RF TRX mask affect by GNT_BT
	 * Set debug mode on: GNT_BT=0, GNT_WL=1, BT at BTG
	 */
	if (coex_sta->kt_ver == 0 &&
	    coex_sta->wl_coex_mode == BTC_WLINK_5G)
		val = 0x40021;
	else if (coex_sta->coex_freerun) /* WL S1 force to GNT_WL=1, GNT_BT=0 */
		val = 0x40021;
	else
		val = 0x40000;

	if (btc->board_info.btdm_ant_num == 1) /* BT at S1 for 2-Ant */
		val = val | BIT(13);

	btc->btc_set_rf_reg(btc, BTC_RF_B, 0x1, 0xfffff, val);

	/* Because WL-S0 2G RF TRX can't masked by GNT_BT
	 * enable "WLS0 BB chage RF mode if GNT_BT = 1" for shared-antenna type
	 * disable:0x1860[3] = 1, enable:0x1860[3] = 0
	 *
	 * enable "AFE DAC off if GNT_WL = 0"
	 * disable 0x1c30[22] = 0,
	 * enable: 0x1c30[22] = 1, 0x1c38[12] = 0, 0x1c38[28] = 1
	 */
	if (coex_sta->wl_coex_mode == BTC_WLINK_2GFREE) {
		btc->btc_write_1byte_bitmask(btc, 0x1c32, BIT(6), 0);
	} else {
		btc->btc_write_1byte_bitmask(btc, 0x1c32, BIT(6), 1);
		btc->btc_write_1byte_bitmask(btc, 0x1c39, BIT(4), 0);
		btc->btc_write_1byte_bitmask(btc, 0x1c3b, BIT(4), 1);
	}

	/* disable WLS1 BB chage RF mode if GNT_BT
	 * since RF TRx mask can do it
	 */
	btc->btc_write_1byte_bitmask(btc, 0x4160, BIT(3), 1);

	/* for kt_ver >= 3: 0x1860[3] = 0
	 * always set "WLS0 BB chage RF mode if GNT_WL = 0"
	 * But the BB DAC will be turned off by GNT_BT = 1
	 * 0x1ca7[3] = 1, "don't off BB DAC if GNT_BT = 1"
	 */
	if (coex_sta->wl_coex_mode == BTC_WLINK_2GFREE) {
		btc->btc_write_1byte_bitmask(btc, 0x1860, BIT(3), 1);
		btc->btc_write_1byte_bitmask(btc, 0x1ca7, BIT(3), 1);
	} else if (coex_sta->wl_coex_mode == BTC_WLINK_5G ||
		   link_info_ext->is_all_under_5g) {
		if (coex_sta->kt_ver >= 3) {
			btc->btc_write_1byte_bitmask(btc, 0x1860, BIT(3), 0);
			btc->btc_write_1byte_bitmask(btc, 0x1ca7, BIT(3), 1);
		} else {
			btc->btc_write_1byte_bitmask(btc, 0x1860, BIT(3), 1);
		}
	} else if (btc->board_info.btdm_ant_num == 2 ||
		   coex_sta->wl_coex_mode == BTC_WLINK_25GMPORT) {
		/* non-shared-antenna or MCC-2band */
		if (coex_sta->kt_ver >= 3) {
			btc->btc_write_1byte_bitmask(btc, 0x1860, BIT(3), 0);
			btc->btc_write_1byte_bitmask(btc, 0x1ca7, BIT(3), 1);
		} else {
			btc->btc_write_1byte_bitmask(btc, 0x1860, BIT(3), 1);
		}
	} else { /* shared-antenna */
		btc->btc_write_1byte_bitmask(btc, 0x1860, BIT(3), 0);
		if (coex_sta->kt_ver >= 3)
			btc->btc_write_1byte_bitmask(btc, 0x1ca7, BIT(3), 0);
	}
}

void halbtc8822e_cfg_gnt_debug(struct btc_coexist *btc)
{
	btc->btc_write_1byte_bitmask(btc, 0x66, BIT(4), 0);
	btc->btc_write_1byte_bitmask(btc, 0x67, BIT(0), 0);
	btc->btc_write_1byte_bitmask(btc, 0x42, BIT(3), 0);
	btc->btc_write_1byte_bitmask(btc, 0x65, BIT(7), 0);
	/* btc->btc_write_1byte_bitmask(btc, 0x73, BIT(3), 1); */
}

void halbtc8822e_cfg_rfe_type(struct btc_coexist *btc)
{
	struct btc_coex_sta *coex_sta = &btc->coex_sta;
	struct btc_rfe_type *rfe_type = &btc->rfe_type;
	struct btc_board_info *board_info = &btc->board_info;

	rfe_type->rfe_module_type = board_info->rfe_type;
	rfe_type->ant_switch_polarity = 0;
	rfe_type->ant_switch_exist = FALSE;
	rfe_type->ant_switch_with_bt = FALSE;
	rfe_type->ant_switch_type = BTC_SWITCH_NONE;
	rfe_type->ant_switch_diversity = FALSE;

	rfe_type->band_switch_exist = FALSE;
	rfe_type->band_switch_type = 0;
	rfe_type->band_switch_polarity = 0;

	if (btc->board_info.btdm_ant_num == 1)
		rfe_type->wlg_at_btg = TRUE;
	else
		rfe_type->wlg_at_btg = FALSE;

	coex_sta->rf4ce_en = FALSE;

	/* Disable LTE Coex Function in WiFi side */
	btc->btc_write_linderct(btc, 0x38, BIT(7), 0);

	/* BTC_CTT_WL_VS_LTE  */
	btc->btc_write_linderct(btc, 0xa0, 0xffff, 0xffff);

	/*  BTC_CTT_BT_VS_LTE */
	btc->btc_write_linderct(btc, 0xa4, 0xffff, 0xffff);
}

void halbtc8822e_cfg_coexinfo_hw(struct btc_coexist *btc)
{
	u8 *cli_buf = btc->cli_buf, u8tmp[4];
	u16 u16tmp[4];
	u32 u32tmp[4];
	boolean lte_coex_on = FALSE;

	u32tmp[0] = btc->btc_read_linderct(btc, 0x38);
	u32tmp[1] = btc->btc_read_linderct(btc, 0x54);
	u8tmp[0] = btc->btc_read_1byte(btc, 0x73);
	lte_coex_on = ((u32tmp[0] & BIT(7)) >> 7) ? TRUE : FALSE;

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/ %s",
		   "LTE Coex/Path Owner", ((lte_coex_on) ? "On" : "Off"),
		   ((u8tmp[0] & BIT(2)) ? "WL" : "BT"));
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = RF:%s_BB:%s/ RF:%s_BB:%s/ %s",
		   "GNT_WL_Ctrl/GNT_BT_Ctrl/Dbg",
		   ((u32tmp[0] & BIT(12)) ? "SW" : "HW"),
		   ((u32tmp[0] & BIT(8)) ? "SW" : "HW"),
		   ((u32tmp[0] & BIT(14)) ? "SW" : "HW"),
		   ((u32tmp[0] & BIT(10)) ? "SW" : "HW"),
		   ((u8tmp[0] & BIT(3)) ? "On" : "Off"));
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d",
		   "GNT_WL/GNT_BT", (int)((u32tmp[1] & BIT(2)) >> 2),
		   (int)((u32tmp[1] & BIT(3)) >> 3));
	CL_PRINTF(cli_buf);

	u32tmp[0] = btc->btc_read_4byte(btc, 0x1c38);
	u8tmp[0] = btc->btc_read_1byte(btc, 0x1860);
	u8tmp[1] = btc->btc_read_1byte(btc, 0x4160);
	u8tmp[2] = btc->btc_read_1byte(btc, 0x1c32);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %d/ %d/ %d/ %d",
		   "1860[3]/4160[3]/1c30[22]/1c38[28]",
		   (int)((u8tmp[0] & BIT(3)) >> 3),
		   (int)((u8tmp[1] & BIT(3)) >> 3),
		   (int)((u8tmp[2] & BIT(6)) >> 6),
		   (int)((u32tmp[0] & BIT(28)) >> 28));
	CL_PRINTF(cli_buf);

	u32tmp[0] = btc->btc_read_4byte(btc, 0x430);
	u32tmp[1] = btc->btc_read_4byte(btc, 0x434);
	u16tmp[0] = btc->btc_read_2byte(btc, 0x42a);
	u16tmp[1] = btc->btc_read_1byte(btc, 0x454);
	u8tmp[0] = btc->btc_read_1byte(btc, 0x426);
	u8tmp[1] = btc->btc_read_1byte(btc, 0x45e);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x/ 0x%x/ 0x%x",
		   "430/434/42a/426/45e[3]/454",
		   u32tmp[0], u32tmp[1], u16tmp[0], u8tmp[0],
		   (int)((u8tmp[1] & BIT(3)) >> 3), u16tmp[1]);
	CL_PRINTF(cli_buf);

	u32tmp[0] = btc->btc_read_4byte(btc, 0x4c);
	u8tmp[2] = btc->btc_read_1byte(btc, 0x64);
	u8tmp[0] = btc->btc_read_1byte(btc, 0x4c6);
	u8tmp[1] = btc->btc_read_1byte(btc, 0x40);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x/ 0x%x",
		   "4c[24:23]/64[0]/4c6[4]/40[5]/RF_0x1",
		   (int)(u32tmp[0] & (BIT(24) | BIT(23))) >> 23, u8tmp[2] & 0x1,
		   (int)((u8tmp[0] & BIT(4)) >> 4),
		   (int)((u8tmp[1] & BIT(5)) >> 5),
		   (int)(btc->btc_get_rf_reg(btc, BTC_RF_B, 0x1, 0xfffff)));
	CL_PRINTF(cli_buf);

	u32tmp[0] = btc->btc_read_4byte(btc, 0x550);
	u8tmp[0] = btc->btc_read_1byte(btc, 0x522);
	u8tmp[1] = btc->btc_read_1byte(btc, 0x953);
	u8tmp[2] = btc->btc_read_1byte(btc, 0xc50);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = 0x%x/ 0x%x/ %s/ 0x%x",
		   "550/522/4-RxAGC/c50", u32tmp[0], u8tmp[0],
		   (u8tmp[1] & 0x2) ? "On" : "Off", u8tmp[2]);
	CL_PRINTF(cli_buf);
}

void halbtc8822e_cfg_wl_tx_power(struct btc_coexist *btc)
{
	struct btc_coex_dm *coex_dm = &btc->coex_dm;

	btc->btc_reduce_wl_tx_power(btc, coex_dm->cur_wl_pwr_lvl);
}

void halbtc8822e_set_wl_lna2(struct btc_coexist *btc, u8 level)
{
	struct btc_coex_dm *coex_dm = &btc->coex_dm;
	u32 i;
	u64 start_time = 0, process_time = 0;

	/* level=0 TIA 3/2, level=1: TIA 1/0
	 * TIA3 = LNA2(0db)+TIA(28db) = 28 db,  TIA2 = LNA2(0db)+TIA(10db) = 10db
	 * TIA1 = LNA2(-8db)+TIA(28db) = 20 db TIA0 = LNA2(-8db)+TIA(10db) = 2db
	 * Note : TIA1 & TIA0 use LNA2(-8db) to reduce WL LO leakage to BT
	 */
	switch (level) {
	case 0:
		start_time = (u64)rtw_get_current_time();
		if (btc->board_info.btdm_ant_num == 1) {
			for (i = 0; i < ARRAY_SIZE(btc_8822e_bt_idle_radioa); i= i+2) {
				btc->btc_set_rf_reg(btc, BTC_RF_A, btc_8822e_bt_idle_radioa[i],
					MASK20BITS, btc_8822e_bt_idle_radioa[i+1]);
			}
			for (i = 0; i < ARRAY_SIZE(btc_8822e_bt_idle_radiob); i= i+2) {
				btc->btc_set_rf_reg(btc, BTC_RF_B, btc_8822e_bt_idle_radiob[i],
					MASK20BITS, btc_8822e_bt_idle_radiob[i+1]);
			}
		}
		process_time = rtw_get_passing_time_ms((u32)start_time);

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		"[BTCoex], %s(): set lna2 rf reg process time = %lld ms\n",
			__func__, process_time);
		BTC_TRACE(trace_buf);

		/* BB set gain table according to BT idle */
		btc->btc_phydm_set_agc_table(btc, FALSE);

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], lna2 setting for BT idle!!\n");
		BTC_TRACE(trace_buf);
		break;
	case 1:
		start_time = (u64)rtw_get_current_time();
		if (btc->board_info.btdm_ant_num == 1) {
			for (i = 0; i < ARRAY_SIZE(btc_8822e_bt_connect_radioa); i= i+2) {
				btc->btc_set_rf_reg(btc, BTC_RF_A, btc_8822e_bt_connect_radioa[i],
					MASK20BITS, btc_8822e_bt_idle_radioa[i+1]);
			}
			for (i = 0; i < ARRAY_SIZE(btc_8822e_bt_connect_radiob); i= i+2) {
				btc->btc_set_rf_reg(btc, BTC_RF_B, btc_8822e_bt_connect_radiob[i],
					MASK20BITS, btc_8822e_bt_connect_radiob[i+1]);
			}
		}
		process_time = rtw_get_passing_time_ms((u32)start_time);

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		"[BTCoex], %s(): set lna2 rf reg process time = %lld ms\n",
			__func__, process_time);
		BTC_TRACE(trace_buf);

		/* BB set gain table according to BT connect */
		btc->btc_phydm_set_agc_table(btc, TRUE);

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], lna2 setting for BT connect!!\n");
		BTC_TRACE(trace_buf);
		break;
	}
}

void halbtc8822e_cfg_wl_rx_gain(struct btc_coexist *btc)
{
	struct btc_coex_dm *coex_dm = &btc->coex_dm;
	struct btc_wifi_link_info_ext *link_info_ext = &btc->wifi_link_info_ext;
	u8 i;

	/* WL Rx Low gain on  */
	static const u32	wl_rx_gain_on_HT20[] = {0xff000003,
		0xbd120003, 0xbe100003, 0xbf080003, 0xbf060003, 0xbf050003,
		0xbc140003, 0xbb160003, 0xba180003, 0xb91a0003, 0xb81c0003,
		0xb71e0003, 0xb4200003, 0xb5220003, 0xb4240003, 0xb3260003,
		0xb2280003, 0xb12a0003, 0xb02c0003, 0xaf2e0003, 0xae300003,
		0xad320003, 0xac340003, 0xab360003, 0x8d380003, 0x8c3a0003,
		0x8b3c0003, 0x8a3e0003, 0x6e400003, 0x6d420003, 0x6c440003,
		0x6b460003, 0x6a480003, 0x694a0003, 0x684c0003, 0x674e0003,
		0x66500003, 0x65520003, 0x64540003, 0x64560003, 0x007e0403};

	static const u32	wl_rx_gain_on_HT40[] = {0xff000003,
		0xbd120003, 0xbe100003, 0xbf080003, 0xbf060003, 0xbf050003,
		0xbc140003, 0xbb160003, 0xba180003, 0xb91a0003, 0xb81c0003,
		0xb71e0003, 0xb4200003, 0xb5220003, 0xb4240003, 0xb3260003,
		0xb2280003, 0xb12a0003, 0xb02c0003, 0xaf2e0003, 0xae300003,
		0xad320003, 0xac340003, 0xab360003, 0x8d380003, 0x8c3a0003,
		0x8b3c0003, 0x8a3e0003, 0x6e400003, 0x6d420003, 0x6c440003,
		0x6b460003, 0x6a480003, 0x694a0003, 0x684c0003, 0x674e0003,
		0x66500003, 0x65520003, 0x64540003, 0x64560003, 0x007e0403};

	/* WL Rx Low gain off  */
	static const u32	wl_rx_gain_off_HT20[] = {0xff000003,
		0xf4120003, 0xf5100003, 0xf60e0003, 0xf70c0003, 0xf80a0003,
		0xf3140003, 0xf2160003, 0xf1180003, 0xf01a0003, 0xef1c0003,
		0xee1e0003, 0xed200003, 0xec220003, 0xeb240003, 0xea260003,
		0xe9280003, 0xe82a0003, 0xe72c0003, 0xe62e0003, 0xe5300003,
		0xc8320003, 0xc7340003, 0xc6360003, 0xc5380003, 0xc43a0003,
		0xc33c0003, 0xc23e0003, 0xc1400003, 0xc0420003, 0xa5440003,
		0xa4460003, 0xa3480003, 0xa24a0003, 0xa14c0003, 0x834e0003,
		0x82500003, 0x81520003, 0x80540003, 0x65560003, 0x007e0403};

	static const u32	wl_rx_gain_off_HT40[] = {0xff000003,
		0xf4120003, 0xf5100003, 0xf60e0003, 0xf70c0003, 0xf80a0003,
		0xf3140003, 0xf2160003, 0xf1180003, 0xf01a0003, 0xef1c0003,
		0xee1e0003, 0xed200003, 0xec220003, 0xeb240003, 0xea260003,
		0xe9280003, 0xe82a0003, 0xe72c0003, 0xe62e0003, 0xe5300003,
		0xc8320003, 0xc7340003, 0xc6360003, 0xc5380003, 0xc43a0003,
		0xc33c0003, 0xc23e0003, 0xc1400003, 0xc0420003, 0xa5440003,
		0xa4460003, 0xa3480003, 0xa24a0003, 0xa14c0003, 0x834e0003,
		0x82500003, 0x81520003, 0x80540003, 0x65560003, 0x007e0403};

	u32		*wl_rx_gain_on, *wl_rx_gain_off;

	if (coex_dm->cur_wl_rx_low_gain_en) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Hi-Li Table On!\n");
		BTC_TRACE(trace_buf);
#if 0
		if (link_info_ext->wifi_bw == BTC_WIFI_BW_HT40)
			wl_rx_gain_on = wl_rx_gain_on_HT40;
		else
			wl_rx_gain_on = wl_rx_gain_on_HT20;
		for (i = 0; i < ARRAY_SIZE(wl_rx_gain_on); i++)
			btc->btc_write_4byte(btc, 0x1d90, wl_rx_gain_on[i]);

		/* set Rx filter corner RCK offset */
		btc->btc_set_rf_reg(btc, BTC_RF_A, 0xde, 0xfffff, 0x22);
		btc->btc_set_rf_reg(btc, BTC_RF_A, 0x1d, 0xfffff, 0x36);
		btc->btc_set_rf_reg(btc, BTC_RF_B, 0xde, 0xfffff, 0x22);
		btc->btc_set_rf_reg(btc, BTC_RF_B, 0x1d, 0xfffff, 0x36);
#endif
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Hi-Li Table Off!\n");
		BTC_TRACE(trace_buf);

#if 0
		if (link_info_ext->wifi_bw == BTC_WIFI_BW_HT40)
			wl_rx_gain_off = wl_rx_gain_off_HT40;
		else
			wl_rx_gain_off = wl_rx_gain_off_HT20;
		for (i = 0; i < ARRAY_SIZE(wl_rx_gain_off); i++)
			btc->btc_write_4byte(btc, 0x1d90, wl_rx_gain_off[i]);

		/* set Rx filter corner RCK offset */
		btc->btc_set_rf_reg(btc, BTC_RF_A, 0xde, 0xfffff, 0x20);
		btc->btc_set_rf_reg(btc, BTC_RF_A, 0x1d, 0xfffff, 0x0);
		btc->btc_set_rf_reg(btc, BTC_RF_B, 0xde, 0xfffff, 0x20);
		btc->btc_set_rf_reg(btc, BTC_RF_B, 0x1d, 0xfffff, 0x0);
#endif

	}

	halbtc8822e_set_wl_lna2(btc, coex_dm->lna2_level);
}

void halbtc8822e_cfg_wlan_act_ips(struct btc_coexist *btc)
{}

void halbtc8822e_cfg_bt_ctrl_act(struct btc_coexist *btc)
{}

void halbtc8822e_chip_setup(struct btc_coexist *btc, u8 type)
{
	switch (type) {
	case BTC_CSETUP_INIT_HW:
		halbtc8822e_cfg_init(btc);
		break;
	case BTC_CSETUP_ANT_SWITCH:
		halbtc8822e_cfg_ant_switch(btc);
		break;
	case BTC_CSETUP_GNT_FIX:
		halbtc8822e_cfg_gnt_fix(btc);
		break;
	case BTC_CSETUP_GNT_DEBUG:
		halbtc8822e_cfg_gnt_debug(btc);
		break;
	case BTC_CSETUP_RFE_TYPE:
		halbtc8822e_cfg_rfe_type(btc);
		break;
	case BTC_CSETUP_COEXINFO_HW:
		halbtc8822e_cfg_coexinfo_hw(btc);
		break;
	case BTC_CSETUP_WL_TX_POWER:
		halbtc8822e_cfg_wl_tx_power(btc);
		break;
	case BTC_CSETUP_WL_RX_GAIN:
		halbtc8822e_cfg_wl_rx_gain(btc);
		break;
	case BTC_CSETUP_WLAN_ACT_IPS:
		halbtc8822e_cfg_wlan_act_ips(btc);
		break;
	case BTC_CSETUP_BT_CTRL_ACT:
		halbtc8822e_cfg_bt_ctrl_act(btc);
		break;
	}
}
#endif
