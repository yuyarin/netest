
class Netest
	
	def initialize
		@scenario = {}
		@test_queue = []
	end
	
	def load_scenario scenario
		@scenario = scenario
	end
	
	def do_test
		@scenario.each do |target, test_cases|
			@test_queue << {
				target: NetestTarget.new(
					target: target.to_s,
					test_cases: test_cases
				),
				thread: nil
			}
		end
		@test_queue.each do |test|
			test[:thread] = Thread.new{ test[:target].do_test }
		end
		@test_queue.each do |test|
			test[:thread].join
			$semaphore.synchronize {
				test[:target].print
			}
		end
	end
end
