// Copyright 2014 theaigames.com (developers@theaigames.com)

//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at

//        http://www.apache.org/licenses/LICENSE-2.0

//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//	
//    For the full copyright and license information, please view the LICENSE
//    file that was distributed with this source code.

package nl.starapple.poker;

public class PreMoveInfo
{
	private int maxWinPot;
	private int amountToCall;
	
	public PreMoveInfo(int maxPot, int amountCall)
	{
		maxWinPot = maxPot;
		amountToCall = amountCall;
	}
	
	/**
	 * Returns a String representation of the current table situation.
	 */
	public String toString()
	{
		String str = "";
		
		str += String.format("Match max_win_pot %d\n", maxWinPot);
		str += String.format("Match amount_to_call %d", amountToCall);
		
		return str;
	}
}
