
class Ping6
	attr_accessor :result
	def initialize(host: "2001:380:6fc::4", size: 64, count: 3, expect: 'rtt < 100')
		@host = host
		@size = size.to_i
		@count = count.to_i
		@expect = expect
		@result = ''
	end
	def description
		"PING IPv6 #{@host} with size #{@size}, expect #{@expect}"
	end
	def test
		result = `ping6 -c #{@count} -i 3 -s #{@size} #{@host} 2>&1 | egrep "time"`
		success_count = 0
		failed = false
		result.each_line do |line|
			if /time=([\d\.]+) ms/ =~ line
				success_count += 1
				rtt_s = $1
				rtt = $1.to_f
				unless eval @expect
					line = line.gsub(rtt_s, "\e[31m#{rtt_s}\e[0m")
					failed = true
				end
			end
			@result += line
		end
		success_count == @count and !failed
	end
end

