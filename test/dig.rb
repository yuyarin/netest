class Dig
	attr_accessor :rr, :proto, :domain, :result, :output
	def initialize(rr: "A", proto: 4, domain: 'www.yuyarin.net', expect: 'ans =~ /221.189.114.163/ and status == "NOERROR"')
		raise "unknown RR: #{rr}" unless ['A', 'AAAA', 'NS', 'MX', 'PTR'].include?(rr)
		@rr = rr
		raise "unknown protocol: #{proto}" unless ['4', '6', 4, 6].include?(proto)
		@proto = proto
		@domain = domain
		@expect = expect
		@result = false
		@output = ''
	end
	def description
		"Dig IPv#{@proto} #{@rr} #{@domain}, expect #{@expect}"
	end
	def do_test
		target = @rr
		#target = '-x' if @rr == 'PTR'
		@output = `dig -#{@proto} #{target} #{@domain} 2>&1 | egrep "#{@domain}|status"`
		noerror = false
		ans = ''
		status = ''
		qualified = false
		@output.each_line do |line|
			if /status: ([^\s,]+)/ =~ line
				status = $1
			end
			ary = line.split(/\s+/)
			ans = ary[4] if ary[3]==@rr
			if eval @expect
				qualified |=  true
			end
		end
		@result = qualified
	end
end
