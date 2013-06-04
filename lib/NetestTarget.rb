
class NetestTarget
	
	def initialize(target: nil, test_cases: [])
		@target = target
		@test_cases = test_cases
		@test_queue = []
		# check_target
	end
	
	def print
		puts "#{@target.capitalize}"
		@test_queue.each do |test|
			result_str = test[:subject].result ? "\e[32m[OK]\e[0m" : "\e[31m[FAIL]\e[0m"
			puts "  #{test[:subject].description} #{result_str}"
			test[:subject].output.each_line do |line|
				puts "    #{line}"
			end
		end
	end
	
	def do_test
		test_module = @target.capitalize
		require "#{File.dirname($0)}/test/#{@target}.rb"
		@test_cases.each do |test_case|
			subject = Object.const_get(test_module).new(test_case)
			puts subject.description
			test = {
				subject: subject,
				thread: nil,
			}
			@test_queue << test
			test[:thread] = Thread.new {
				test[:subject].do_test
			}
		end
		@result = true
		@test_queue.each do |test|
			test[:thread].join
			@result &= test[:subject].result
		end
		@result
	end
end
