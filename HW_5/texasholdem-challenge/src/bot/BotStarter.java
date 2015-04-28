/**
 * www.TheAIGames.com 
 * Heads Up Omaha pokerbot
 *
 * Last update: May 07, 2014
 *
 * @author Jim van Eeden, Starapple
 * @version 1.0
 * @License MIT License (http://opensource.org/Licenses/MIT)
 */


package bot;

import poker.Card;
import poker.Hand;
import poker.HandHoldem;
import poker.PokerMove;

import com.stevebrecher.HandEval;

/**
 * This class is the brains of your bot. Make your calculations here and return the best move with GetMove
 */
public class BotStarter implements Bot {

	/**
	 * Implement this method to return the best move you can. Currently it will return a raise the ordinal value
	 * of one of our cards is higher than 9, a call when one of the cards has a higher ordinal value than 5 and
	 * a check otherwise.
	 * @param state : The current state of your bot, with all the (parsed) information given by the engine
	 * @param timeOut : The time you have to return a move
	 * @return PokerMove : The move you will be doing
	 */
	@Override
	public PokerMove getMove(BotState state, Long timeOut) {
        double random=Math.random()*100;//random value between 0 and 100
        System.err.printf("random vale: %f\n",random);
        int raiseAmount= (int)(Math.random()*50*state.getBigBlind());
        //return new PokerMove(state.getMyName(), "call", state.getAmountToCall());/*
		HandHoldem hand = state.getHand();
		String handCategory = getHandCategory(hand, state.getTable()).toString();

        if (state.getTable()==null||state.getTable().length==0) {//before the flop
            if (handCategory.equals(HandEval.HandCategory.PAIR)) {//if we start with a pair
                if (random < 70) {
                    return new PokerMove(state.getMyName(), "raise", (int)(raiseAmount*Math.random()));// 70 percent chance to raise
                }
                if (random < 95) {
                    return new PokerMove(state.getMyName(), "call", state.getAmountToCall());//25 percent chance to call
                }
                if (random < 100) {
                    return new PokerMove(state.getMyName(), "check", 0);//5 percent chance to check
                }
            }
            int height1 = hand.getCard(0).getHeight().ordinal();
            int height2 = hand.getCard(1).getHeight().ordinal();

            // Return the appropriate move according to our amazing strategy
            if( height1 > 9 || height2 > 9 ) {
                if (random < 20) {
                    return new PokerMove(state.getMyName(), "raise", (int)(raiseAmount*Math.random()));// 20 percent chance to raise
                }
                if (random < 80) {
                    return new PokerMove(state.getMyName(), "call", state.getAmountToCall());//60 percent chance to call
                }
                if (random < 100) {
                    return new PokerMove(state.getMyName(), "check", 0);//20 percent chance to check
                }
            } else if( height1 > 5 && height2 > 5 ) {
                if (random < 20) {
                    return new PokerMove(state.getMyName(), "raise", (int)(raiseAmount*Math.random()));// 20 percent chance to raise
                }
                if (random < 70) {
                    return new PokerMove(state.getMyName(), "call", state.getAmountToCall());//50 percent chance to call
                }
                if (random < 100) {
                    return new PokerMove(state.getMyName(), "check", 0);//30 percent chance to check
                }
            } else {
                if (random < 10) {
                    return new PokerMove(state.getMyName(), "raise", (int)(raiseAmount*Math.random()));// 10 percent chance to raise
                }
                if (random < 30) {
                    return new PokerMove(state.getMyName(), "call", state.getAmountToCall());//30 percent chance to call
                }
                if (random < 100) {
                    return new PokerMove(state.getMyName(), "check", 0);//70 percent chance to check
                }
            }

        }
        //after the flop
        if (handCategory.equals(HandEval.HandCategory.FLUSH)||handCategory.equals(HandEval.HandCategory.STRAIGHT_FLUSH)
                || handCategory.equals(HandEval.HandCategory.FOUR_OF_A_KIND)|| handCategory.equals(HandEval.HandCategory.FULL_HOUSE)){
            int oppStack=state.getOpponentStack();
            if (oppStack>200) {
                return new PokerMove(state.getMyName(), "raise", state.getOpponentStack() / 4);//raise a fourth of their stack, entice them
            }
            else
                return new PokerMove(state.getMyName(),"raise",state.getOpponentStack());//if the have a small stack make them go all in
        }
        if (handCategory.equals(HandEval.HandCategory.STRAIGHT)){
            if (random<50){
                return new PokerMove(state.getMyName(),"raise",(int)(raiseAmount*Math.random()));
            }
            if (random<95){
                return new PokerMove(state.getMyName(),"call",state.getAmountToCall());
            }
            return new PokerMove(state.getMyName(),"check",0);
        }
        if (handCategory.equals(HandEval.HandCategory.THREE_OF_A_KIND)||handCategory.equals(HandEval.HandCategory.TWO_PAIR)){
            if (random<30){
                return new PokerMove(state.getMyName(),"raise",(int)(raiseAmount*Math.random()));
            }
            if (random<60){
                return new PokerMove(state.getMyName(),"call",state.getAmountToCall());
            }
            return new PokerMove(state.getMyName(),"check",0);
        }

        if (random<70) {
            return new PokerMove(state.getMyName(), "check", 0);
        }
        if (random<90){
            return new PokerMove(state.getMyName(),"call",0);
        }
        return new PokerMove(state.getMyName(),"raise",0);
	}
	
	/**
	 * Calculates the bot's hand strength, with 0, 3, 4 or 5 cards on the table.
	 * This uses the com.stevebrecher package to get hand strength.
	 * @param hand : cards in hand
	 * @param table : cards on table
	 * @return HandCategory with what the bot has got, given the table and hand
	 */
	public HandEval.HandCategory getHandCategory(HandHoldem hand, Card[] table) {
		if( table == null || table.length == 0 ) { // there are no cards on the table
			return hand.getCard(0).getHeight() == hand.getCard(1).getHeight() // return a pair if our hand cards are the same
					? HandEval.HandCategory.PAIR
					: HandEval.HandCategory.NO_PAIR;
		}
		long handCode = hand.getCard(0).getNumber() + hand.getCard(1).getNumber();
		
		for( Card card : table ) { handCode += card.getNumber(); }
		
		if( table.length == 3 ) { // three cards on the table
			return rankToCategory(HandEval.hand5Eval(handCode));
		}
		if( table.length == 4 ) { // four cards on the table
			return rankToCategory(HandEval.hand6Eval(handCode));
		}
		return rankToCategory(HandEval.hand7Eval(handCode)); // five cards on the table
	}
	
	/**
	 * small method to convert the int 'rank' to a readable enum called HandCategory
	 */
	public HandEval.HandCategory rankToCategory(int rank) {
		return HandEval.HandCategory.values()[rank >> HandEval.VALUE_SHIFT];
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		BotParser parser = new BotParser(new BotStarter());
		parser.run();
	}

}
