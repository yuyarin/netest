
class Tcping
	
	attr_accessor :result, :output
	
	def initialize(host: "221.189.114.163", port: 80, proto: 4, expect: 'status=="open"', tcping: '/usr/local/bin/tcping')
		@host = host
		@port = port.to_i
		raise "unknown IP version" unless ['4', '6', 4, 6].include?(proto)
		@proto = proto
		@expect = expect
		@result = false
		@output = ''
		raise "tcping does not exist, install it following README.md" unless File.exists? tcping
		@tcping = tcping
	end
	
	def description
		"TCP PING IPv#{@proto} #{@host} #{@port}, expect #{@expect}"
	end
	
	def do_test
		output = `#{@tcping} -t 1 -p#{@proto} #{@host} #{@port} 2>&1`
		failed = false
		status = ''
		rtt = 0
		output.each_line do |line|
			if /status:([^\s]+)/ =~ line
				status = $1
			end
			if /rtt:([^\s]+)/ =~ line
				rtt = $1.to_f
			end
			@output += line
		end
		@result = eval @expect
	end
end

