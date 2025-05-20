enum NotificationId {
    c_notificationType0 = 0,
    c_notificationStartAction = 1, // 100dc210:100d8350
    c_notificationEndAction = 2,   // 100d8358:100d8350
    c_notificationType4 = 4,       // 100dc208:100d8350
    c_notificationPresenter = 5,
    c_notificationStreamer = 6,   // 100dc760
    c_notificationKeyPress = 7,   // 100d6aa0
    c_notificationButtonUp = 8,   // 100d6aa0
    c_notificationButtonDown = 9, // 100d6aa0
    c_notificationMouseMove = 10, // 100d6aa0
    c_notificationClick = 11,     // 100d6aa0
    c_notificationDragStart = 12,
    c_notificationDrag = 13,
    c_notificationDragEnd = 14,
    c_notificationTimer = 15, // 100d6aa0
    c_notificationControl = 17,
    c_notificationEndAnim = 18,    // 100d7e80
    c_notificationPathStruct = 19, // 100d6230
    c_notificationType20 = 20,
    c_notificationNewPresenter = 21,
    c_notificationType22 = 22,
    c_notificationType23 = 23,
    c_notificationTransitioned = 24
}

enum Area {
    e_undefined = 0,
    e_previousArea = 0,
    e_isle,
    e_infomain,
    e_infodoor,
    e_unk4,
    e_elevbott,
    e_elevride,
    e_elevride2,
    e_elevopen,
    e_seaview,
    e_observe,
    e_elevdown,
    e_regbook,
    e_infoscor,
    e_jetrace,
    e_jetrace2,
    e_jetraceExterior,
    e_unk17,
    e_carrace,
    e_carraceExterior,
    e_unk20,
    e_unk21,
    e_pizzeriaExterior,
    e_unk23,
    e_unk24,
    e_garageExterior,
    e_garage,
    e_garadoor,
    e_unk28,
    e_hospitalExterior,
    e_hospital,
    e_unk31,
    e_policeExterior,
    e_unk33,
    e_police,
    e_polidoor,
    e_copterbuild,
    e_dunecarbuild,
    e_jetskibuild,
    e_racecarbuild,
    e_unk40,
    e_unk41,
    e_unk42,
    e_unk43,
    e_unk44,
    e_unk45,
    e_act2main,
    e_act3script,
    e_unk48,
    e_unk49,
    e_unk50,
    e_unk51,
    e_unk52,
    e_jukeboxw,
    e_jukeboxExterior,
    e_unk55,
    e_histbook,
    e_bike,
    e_dunecar,
    e_motocycle,
    e_copter,
    e_skateboard,
    e_ambulance,
    e_towtrack,
    e_jetski,

    e_unk66 = 66
};


enum ActionType {
    e_none = 0,
    e_opendisk,
    e_openram,
    e_close,
    e_start,
    e_stop,
    e_run,
    e_exit,
    e_enable,
    e_disable,
    e_notify,
    e_unknown,
};
