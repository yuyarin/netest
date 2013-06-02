class Tcping
	attr_accessor :result
	def initialize(host: "221.189.114.163", port: 80, proto: 4, expect: 'status=="open"')
		@host = host
		@port = port.to_i
		raise "unknown IP version" unless ['4', '6', 4, 6].include?(proto)
		@proto = proto
		@expect = expect
		@result = ''
	end
	def description
		"TCP PING IPv#{@proto} #{@host} #{@port}, expect #{@expect}"
	end
	def test
		result = `/Users/yuyarin/scripts/tcping -p#{@proto} #{@host} #{@port} 2>&1`
		failed = false
		status = ''
		rtt = 0
		result.each_line do |line|
			if /status:([^\s]+)/ =~ line
				status = $1
			end
			if /rtt:([^\s]+)/ =~ line
				rtt = $1.to_f
			end
			@result += line
		end
		eval @expect
	end
end
