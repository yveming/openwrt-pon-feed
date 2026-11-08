// SPDX-License-Identifier: Apache-2.0

'use strict';
'require form';
'require fs';
'require network';
'require uci';
'require view';

function validateIfname(sectionId, value) {
	if (!/^[A-Za-z0-9_.:-]{1,15}$/.test(value || ''))
		return _('Use a Linux network device name containing at most 15 characters.');

	return true;
}

var DEFAULT_CONFIG =
	"config iptv 'config'\n" +
	"\toption enabled '0'\n" +
	"\toption uplink 'pon0'\n" +
	"\toption name 'itv'\n" +
	"\toption service_vid ''\n" +
	"\toption mcast_vid ''\n" +
	"\toption macaddr ''\n";

return view.extend({
	load: function() {
		return Promise.all([
			fs.list('/etc/config').then(function(reply) {
				var entries = (reply && reply.entries) || [];

				if (!entries.some(function(e) { return e.name == 'iptv'; }))
					return fs.write('/etc/config/iptv', DEFAULT_CONFIG);
			}).then(function() {
				uci.unload('iptv');
				return uci.load('iptv');
			}).then(function() {
				if (!uci.get('iptv', 'config'))
					uci.add('iptv', 'iptv', 'config');
			}).catch(function() {
				/* The file/ACL may be unavailable; keep the page usable
				 * and let the form create the section client-side. */
				uci.unload('iptv');
				return uci.load('iptv').catch(function() {
					uci.add('iptv', 'iptv', 'config');
				});
			}),
			network.getDevices().catch(function() { return []; })
		]);
	},

	render: function(data) {
		var devices = (data && data[1]) || [];
		var names = devices.map(function(dev) { return dev.getName(); });
		var m, s, o;

		m = new form.Map('iptv', _('IPTV'),
			_('Create an untagged IPTV device carrying the service and multicast VLANs over the uplink. Add the device to a bridge or interface in Network → Interfaces if needed.'));

		s = m.section(form.NamedSection, 'config', 'iptv');
		s.anonymous = true;
		s.addremove = false;

		o = s.option(form.Flag, 'enabled', _('Enable'));
		o.default = '0';
		o.rmempty = false;

		o = s.option(form.ListValue, 'uplink', _('Uplink device'),
			_('Network device carrying the operator VLANs, normally pon0.'));
		names.forEach(function(name) {
			if (name != 'lo')
				o.value(name);
		});

		var current = uci.get('iptv', 'config', 'uplink') || 'pon0';
		if (names.indexOf(current) < 0)
			o.value(current);
		o.default = 'pon0';
		o.rmempty = false;
		o.depends('enabled', '1');

		o = s.option(form.Value, 'name', _('Device name'),
			_('Linux name of the IPTV device. Add it to a bridge or interface in Network → Interfaces if needed.'));
		o.default = 'itv';
		o.rmempty = false;
		o.validate = validateIfname;
		o.depends('enabled', '1');

		o = s.option(form.Value, 'service_vid', _('Service VLAN'),
			_('Operator VLAN carrying IPTV unicast traffic.'));
		o.datatype = 'range(1,4094)';
		o.rmempty = false;
		o.depends('enabled', '1');

		o = s.option(form.Value, 'mcast_vid', _('Multicast VLAN'),
			_('Operator VLAN carrying IPTV multicast traffic.'));
		o.datatype = 'range(1,4094)';
		o.rmempty = false;
		o.depends('enabled', '1');
		o.validate = function(sectionId, value) {
			var serviceVid = this.section.formvalue(sectionId, 'service_vid');

			if (value && serviceVid && value === serviceVid)
				return _('The multicast VLAN must differ from the service VLAN.');

			return true;
		};

		o = s.option(form.Value, 'macaddr', _('MAC address'),
			_('Optional. Override the MAC address of the IPTV device.'));
		o.datatype = 'macaddr';
		o.rmempty = true;
		o.depends('enabled', '1');

		return m.render();
	}
});
