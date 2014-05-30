
class Ping
	
	attr_accessor :result, :output
	
	def initialize(host: "221.189.114.163", size: 64, count: 3, expect: 'rtt < 100')
		@host = host
		@size = size.to_i
		@count = count.to_i
		@expect = expect
		@result = false
		@output = ''
	end
	
	def description
		"PING IPv4 #{@host} with size #{@size}, expect #{@expect}"
	end
	
	def do_test
		output = `ping -c #{@count} -t 3 #{@host} 2>&1 | egrep "time"`
		success_count = 0
		failed = false
		output.each_line do |line|
			if /time=([\d\.]+) ms/ =~ line
				success_count += 1
				rtt_s = $1
				rtt = $1.to_f
				unless  eval(@expect)
					line = line.gsub(rtt_s, "\e[31m#{rtt_s}\e[0m")
					failed = true
				end
			end
			@output += line
		end
		@result = (success_count == @count and !failed)
	end
end

