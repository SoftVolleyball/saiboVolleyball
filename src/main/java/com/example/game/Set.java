package com.example.game;

import com.example.group.Lineup;
import com.example.group.Team;

public class Set {
    private final Lineup lineupA;
    private final Lineup lineupB;

    private final Lineup servingTeam;

    private int scoreA;
    private int scoreB;

    private final int goalScore;

    public Set(Lineup servingTeam, Lineup receivingTeam, int goalScore) {
        lineupA = servingTeam;
        lineupB = receivingTeam;
        this.servingTeam = servingTeam;
        this.goalScore = goalScore;
        scoreA = scoreB = 0;
    }

    public Team play() {
        Lineup servingLineup = servingTeam;
        while (true) {
            Point point = new Point(servingLineup, servingLineup == lineupA ? lineupB : lineupA);
            Team winner = point.getWinner();

            if (winner == lineupA.getTeam()) scoreA++;
            else if (winner == lineupB.getTeam()) scoreB++;

            Lineup loser = winner == lineupA.getTeam() ? lineupB : lineupA;
            for (int i = 1; i <= 6; i++) {
                winner.getPlayer(i).mentalityChange(1.01);
                loser.getPlayer(i).mentalityChange(0.99);
            }

            if ((scoreA >= goalScore && scoreA - scoreB >=2)
                    || (scoreB >= scoreA && scoreB - scoreA >= 2)) return winner;

            if (winner != servingLineup.getTeam()) {
                servingLineup = servingLineup == lineupA ? lineupB : lineupA;
                servingLineup.rotate();
            }
        }
    }

}
