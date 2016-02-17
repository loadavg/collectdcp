# project collectd_edit
# ing. Carlo Capelli
# Brescia 2015
#
# Copyright (c) 2015,2016 Sputnik7
# License MIT
#

TEMPLATE = app

CONFIG += console
CONFIG += C++11
CONFIG -= qt
CONFIG -= app_bundle

CONFIG += link_pkgconfig
PKGCONFIG += gtkmm-3.0

DEFINES += USE_SOURCEVIEW
PKGCONFIG += gtksourceviewmm-3.0

# Stick to 3-8, glade compatible upto 3-10.
#DEFINES += RESOURCES_DIR=\\\"resources-3.10\\\"
DEFINES += RESOURCES_DIR=\\\"resources-3.8\\\"

SOURCES += main.cpp \
    parse_conf.cpp \
    model.cpp \
    view_ast.cpp \
    ui_structure.cpp \
    message_box.cpp \
    range.cpp \
    dlg_commands.cpp \
    attr_helper.cpp \
    process_run.cpp \
    settings.cpp \
    os_info.cpp \
    plugin_to_store.cpp \
    ast_loader.cpp \
    glob_path_pattern.cpp \
    dir_structure.cpp \
    collectdcp_app.cpp

HEADERS += \
    parse_conf.h \
    model.h \
    file2string.h \
    view_ast.h \
    ui_structure.h \
    message_box.h \
    prints.h \
    icompare.h \
    range.h \
    tokenizer.h \
    join.h \
    dlg_commands.h \
    attr_helper.h \
    process_run.h \
    is_a.h \
    depth_first.h \
    edittext.h \
    ns_all.h \
    settings.h \
    ini_file.h \
    fileuty.h \
    trim.h \
    var_assign.h \
    os_info.h \
    split.h \
    plugin_to_store.h \
    ast_loader.h \
    glob_path_pattern.h \
    dir_structure.h \
    collectdcp_app.h

DISTFILES += \
    Makefile \
    README.md \
    doxygen_config \
    resources-3.8/test_glade.glade \
    resources-3.8/editor.glade \
    resources-3.8/dlg_commands.glade \
    resources-3.8/collectdcp.glade \
    resources-3.8/add_plugin_treeview.glade \
    resources-3.8/about_box.glade \
    resources-3.8/plugins.template \
    resources-3.10/test_glade.glade \
    resources-3.10/collectdcp.glade \
    resources-3.10/editor.glade \
    resources-3.10/dlg_commands.glade \
    resources-3.10/add_plugin_treeview.glade \
    resources-3.10/about_box.glade \
    resources-3.10/plugins.template \
    swipl-plgi/url_page_cache.pl \
    swipl-plgi/parse_conf.pl \
    swipl-plgi/glade_utility.pl \
    swipl-plgi/add_plugin_block.pl \
    swipl-plgi/colors.pl \
    swipl-plgi/dlg_commands.pl \
    swipl-plgi/editapp.pl \
    swipl-plgi/message_box.pl \
    swipl-plgi/model.pl \
    swipl-plgi/scrap_plugins_wiki.pl \
    swipl-plgi/ui_structure.pl \
    swipl-plgi/uty.pl \
    swipl-plgi/view_ast.pl \
    swipl-plgi/ubuntu-14.04-default/collectd.conf.d/thresholds.conf \
    swipl-plgi/ubuntu-14.04-default/collectd.conf.d/filters.conf \
    swipl-plgi/ubuntu-14.04-default/collectd.conf \
    swipl-plgi/ubuntu-14.04-default/collection.conf \
    swipl-plgi/gen_glade.pl \
    resources-3.8/generated/main.glade \
    resources-3.8/generated/aggregation.glade \
    resources-3.8/generated/amqp.glade \
    resources-3.8/generated/apache.glade \
    resources-3.8/generated/apcups.glade \
    resources-3.8/generated/aquaero.glade \
    resources-3.8/generated/ascent.glade \
    resources-3.8/generated/barometer.glade \
    resources-3.8/generated/battery.glade \
    resources-3.8/generated/bind.glade \
    resources-3.8/generated/ceph.glade \
    resources-3.8/generated/cgroups.glade \
    resources-3.8/generated/conntrack.glade \
    resources-3.8/generated/cpu.glade \
    resources-3.8/generated/cpufreq.glade \
    resources-3.8/generated/csv.glade \
    resources-3.8/generated/curl.glade \
    resources-3.8/generated/curl_json.glade \
    resources-3.8/generated/curl_xml.glade \
    resources-3.8/generated/dbi.glade \
    resources-3.8/generated/df.glade \
    resources-3.8/generated/disk.glade \
    resources-3.8/generated/dns.glade \
    resources-3.8/generated/email.glade \
    resources-3.8/generated/ethstat.glade \
    resources-3.8/generated/exec.glade \
    resources-3.8/generated/fhcount.glade \
    resources-3.8/generated/filecount.glade \
    resources-3.8/generated/GenericJMX.glade \
    resources-3.8/generated/gmond.glade \
    resources-3.8/generated/hddtemp.glade \
    resources-3.8/generated/interface.glade \
    resources-3.8/generated/ipmi.glade \
    resources-3.8/generated/iptables.glade \
    resources-3.8/generated/irq.glade \
    resources-3.8/generated/java.glade \
    resources-3.8/generated/load.glade \
    resources-3.8/generated/logfile.glade \
    resources-3.8/generated/log_logstash.glade \
    resources-3.8/generated/lpar.glade \
    resources-3.8/generated/mbmon.glade \
    resources-3.8/generated/md.glade \
    resources-3.8/generated/memcachec.glade \
    resources-3.8/generated/memcached.glade \
    resources-3.8/generated/memory.glade \
    resources-3.8/generated/mic.glade \
    resources-3.8/generated/modbus.glade \
    resources-3.8/generated/mysql.glade \
    resources-3.8/generated/netapp.glade \
    resources-3.8/generated/netlink.glade \
    resources-3.8/generated/network.glade \
    resources-3.8/generated/nginx.glade \
    resources-3.8/generated/notify_desktop.glade \
    resources-3.8/generated/notify_email.glade \
    resources-3.8/generated/ntpd.glade \
    resources-3.8/generated/nut.glade \
    resources-3.8/generated/olsrd.glade \
    resources-3.8/generated/onewire.glade \
    resources-3.8/generated/openldap.glade \
    resources-3.8/generated/openvpn.glade \
    resources-3.8/generated/oracle.glade \
    resources-3.8/generated/perl.glade \
    resources-3.8/generated/pinba.glade \
    resources-3.8/generated/ping.glade \
    resources-3.8/generated/postgresql.glade \
    resources-3.8/generated/powerdns.glade \
    resources-3.8/generated/processes.glade \
    resources-3.8/generated/protocols.glade \
    resources-3.8/generated/python.glade \
    resources-3.8/generated/redis.glade \
    resources-3.8/generated/routeros.glade \
    resources-3.8/generated/rrdcached.glade \
    resources-3.8/generated/rrdtool.glade \
    resources-3.8/generated/sensors.glade \
    resources-3.8/generated/sigrok.glade \
    resources-3.8/generated/smart.glade \
    resources-3.8/generated/snmp.glade \
    resources-3.8/generated/statsd.glade \
    resources-3.8/generated/swap.glade \
    resources-3.8/generated/syslog.glade \
    resources-3.8/generated/table.glade \
    resources-3.8/generated/tail.glade \
    resources-3.8/generated/tail_csv.glade \
    resources-3.8/generated/tcpconns.glade \
    resources-3.8/generated/teamspeak2.glade \
    resources-3.8/generated/ted.glade \
    resources-3.8/generated/thermal.glade \
    resources-3.8/generated/threshold.glade \
    resources-3.8/generated/tokyotyrant.glade \
    resources-3.8/generated/turbostat.glade \
    resources-3.8/generated/unixsock.glade \
    resources-3.8/generated/uuid.glade \
    resources-3.8/generated/varnish.glade \
    resources-3.8/generated/virt.glade \
    resources-3.8/generated/vmem.glade \
    resources-3.8/generated/vserver.glade \
    resources-3.8/generated/write_graphite.glade \
    resources-3.8/generated/write_http.glade \
    resources-3.8/generated/write_kafka.glade \
    resources-3.8/generated/write_mongodb.glade \
    resources-3.8/generated/write_redis.glade \
    resources-3.8/generated/write_riemann.glade \
    resources-3.8/generated/write_sensu.glade \
    resources-3.8/generated/write_tsdb.glade \
    resources-3.8/generated/zookeeper.glade \
    swipl-plgi/xml_ize.pl \
    license.txt \
    resources-3.8/button.css \
    resources-3.8/collectdcp_win.glade \
    resources-3.8/loadavglogo2-small.png
